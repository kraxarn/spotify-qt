#include "mainwindow.hpp"

MainWindow::MainWindow(Settings &settings)
	: settings(settings), QMainWindow()
{
	// Splash
	auto splash = new SplashDialog();
	splash->show();
	splash->showMessage("Please wait...");

	// Set cache root location
	cacheLocation = QStandardPaths::standardLocations(QStandardPaths::CacheLocation)[0];

	// Create main cache path and album subdir
	QDir cacheDir(cacheLocation);
	cacheDir.mkpath(".");
	cacheDir.mkdir("album");
	cacheDir.mkdir("playlist");
	cacheDir.mkdir("tracks");

	// Apply selected style and palette
	QApplication::setStyle(settings.general.style);
	Utils::applyPalette(settings.general.stylePalette);

	// Check for dark background
	auto bg = palette().color(backgroundRole());
	if (((bg.red() + bg.green() + bg.blue()) / 3) < 128)
		Utils::darkBackground = true;

	// Set Spotify
	splash->showMessage("Connecting...");
	spotify = new spt::Spotify(settings, this);
	network = new QNetworkAccessManager();

	// Check connection
	stateValid = spotify->isValid();
	if (!stateValid)
	{
		splash->finish(this);
		QMessageBox::warning(this, "Connection failed",
			"Failed to connect to Spotify, check your connection and try again");
		return;
	}

	// Empty icon used as replacement for play icon
	QPixmap emptyPixmap(64, 64);
	emptyPixmap.fill(Qt::transparent);
	emptyIcon = QIcon(emptyPixmap);

	// Setup main window
	setWindowTitle("spotify-qt");
	setWindowIcon(Icon::get("logo:spotify-qt"));
	resize(1280, 720);
	setCentralWidget(createCentralWidget());
	toolBar = new MainToolBar(*spotify, settings, this);
	addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);

	// Update player status
	splash->showMessage("Refreshing...");
	auto timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout, this, &MainWindow::refresh);
	refresh();
	timer->start(1000);
	splash->showMessage("Welcome!");

	// Check if should start client
	if (settings.spotify.startClient
		&& (settings.spotify.alwaysStart || spotify->devices().isEmpty()))
	{
		sptClient = new spt::ClientHandler(settings, this);
		auto status = sptClient->start();
		if (!status.isEmpty())
			QMessageBox::warning(this, "Client error",
				QString("Failed to autostart Spotify client: %1").arg(status));
	}

	// Start media controller if specified
	if (settings.general.mediaController)
	{
		mediaPlayer = new mp::Service(spotify, this);
		// Check if something went wrong during init
		if (!mediaPlayer->isValid())
		{
			delete mediaPlayer;
			mediaPlayer = nullptr;
		}
	}

	// Start listening to current playback responses
	spt::Spotify::connect(spotify, &spt::Spotify::gotPlayback, [this](const spt::Playback &playback)
	{
		refreshed(playback);
	});

	// Create tray icon if specified
	if (settings.general.trayIcon)
		trayIcon = new TrayIcon(spotify, settings, this);

	// If new version has been detected, show what's new dialog
	if (settings.general.showChangelog
		&& settings.general.lastVersion != APP_VERSION
		&& APP_VERSION != QString("v0.0"))
		(new WhatsNewDialog(APP_VERSION, settings, this))->open();
	settings.general.lastVersion = APP_VERSION;
	settings.save();

	// Welcome
	setStatus("Welcome to spotify-qt!");
	splash->finish(this);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	delete trayIcon;
	event->accept();
}

void MainWindow::refresh()
{
	if (refreshCount < 0
		|| ++refreshCount >= settings.general.refreshInterval
		|| current.progressMs + 1000 > current.item.duration)
	{
		spotify->requestCurrentPlayback();
		refreshCount = 0;
		return;
	}

	// Assume last refresh was 1 sec ago
	if (!current.isPlaying)
		return;
	current.progressMs += 1000;
	refreshed(current);
}

void MainWindow::refreshed(const spt::Playback &playback)
{
	// Just so it doesn't make the app startup slower for now
	if (currentUser.id.isEmpty())
		currentUser = spotify->me();
	current = playback;

	auto mainToolBar = dynamic_cast<MainToolBar *>(toolBar);

	if (!current.isPlaying && current.item.name == "(no name)")
	{
		mainToolBar->playPause->setIcon(Icon::get("media-playback-start"));
		mainToolBar->playPause->setText("Play");
		return;
	}
	auto currPlaying = QString("%1\n%2").arg(current.item.name).arg(current.item.artist);
	if (nowPlaying->text() != currPlaying)
	{
		if (current.isPlaying)
		{
			setPlayingTrackItem(trackItems.contains(current.item.id)
				? trackItems[current.item.id]
				: nullptr);
		}

		nowPlaying->setText(currPlaying);
		setAlbumImage(current.item.image);
		setWindowTitle(QString("%1 - %2").arg(current.item.artist).arg(current.item.name));
		updateContextIcon();
		if (mediaPlayer != nullptr)
			mediaPlayer->currentSourceChanged(current);
		if (trayIcon != nullptr && settings.general.trayAlbumArt)
			trayIcon->setPixmap(getAlbum(current.item.image));
	}
	mainToolBar->position->setText(QString("%1/%2")
		.arg(Utils::formatTime(current.progressMs))
		.arg(Utils::formatTime(current.item.duration)));
	mainToolBar->progress->setValue(current.progressMs);
	mainToolBar->progress->setMaximum(current.item.duration);
	mainToolBar->playPause->setIcon(Icon::get(current.isPlaying
		? "media-playback-pause"
		: "media-playback-start"));
	mainToolBar->playPause->setText(current.isPlaying ? "Pause" : "Play");
	if (!settings.general.pulseVolume)
		mainToolBar->volumeButton->setVolume(current.volume() / 5);
	mainToolBar->repeat->setChecked(current.repeat != "off");
	mainToolBar->shuffle->setChecked(current.shuffle);
}

QWidget *MainWindow::createCentralWidget()
{
	auto container = new QSplitter();
	auto sidebar = new QVBoxLayout();

	//region Library
	libraryList = new LibraryList(*spotify, this);
	auto library = Utils::createGroupBox(QVector<QWidget *>() << libraryList, this);
	library->setTitle("Library");
	sidebar->addWidget(library);
	//endregion

	//region Playlists
	playlists = new PlaylistList(*spotify, this);
	refreshPlaylists();

	auto playlistContainer = Utils::createGroupBox(QVector<QWidget *>() << playlists, this);
	playlistContainer->setTitle("Playlists");
	sidebar->addWidget(playlistContainer);
	//endregion

	// Current context info
	auto contextLayout = new QHBoxLayout();
	contextIcon = new QLabel();
	contextIcon->setVisible(false);
	contextInfo = new QLabel();
	contextInfo->setToolTip("Currently playing from");
	contextInfo->setVisible(false);
	contextLayout->addSpacing(16);
	contextLayout->addWidget(contextIcon);
	contextLayout->addWidget(contextInfo, 1);
	sidebar->addLayout(contextLayout);
	contextInfo->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(contextInfo, &QWidget::customContextMenuRequested,
		this, &MainWindow::contextInfoMenu);

	// Now playing song
	auto nowPlayingLayout = new QHBoxLayout();
	nowPlayingLayout->setSpacing(12);
	nowAlbum = new QLabel(this);
	nowAlbum->setFixedSize(64, 64);
	nowAlbum->setPixmap(Icon::get("media-optical-audio").pixmap(nowAlbum->size()));
	nowPlayingLayout->addWidget(nowAlbum);
	nowPlaying = new QLabel("No music playing", this);
	nowPlaying->setWordWrap(true);
	nowPlayingLayout->addWidget(nowPlaying);
	sidebar->addLayout(nowPlayingLayout);

	// Show menu when clicking now playing
	nowPlaying->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(nowPlaying, &QWidget::customContextMenuRequested, [this](const QPoint &pos)
	{
		auto track = current.item;
		if (track.name.isEmpty() && track.artist.isEmpty())
			return;
		songMenu(track.id, track.artist, track.name, track.artistId, track.albumId)
			->popup(nowPlaying->mapToGlobal(pos));
	});

	// Sidebar as widget
	auto sidebarWidget = Utils::layoutToWidget(sidebar);
	sidebarWidget->setMaximumWidth(250);
	container->addWidget(sidebarWidget);

	//region Songs
	songs = new TracksList(*spotify, settings, this);
	//endregion

	// Load tracks in playlist
	auto playlistId = settings.general.lastPlaylist;
	if (sptPlaylists.isEmpty())
	{
		// If no playlists were found
		// TODO: Load something from library here
	}
	else if (playlistId.isEmpty())
	{
		// Default to first in list
		playlistId = playlists->item(0)->data(RolePlaylistId).toString();
	}

	// Find playlist in list
	for (auto i = 0; i < playlists->count(); i++)
	{
		auto item = playlists->item(i);

		if (item->data(RolePlaylistId).toString().endsWith(playlistId))
		{
			playlists->setCurrentRow(i);
			loadPlaylist(sptPlaylists[item->data(RoleIndex).toInt()]);
		}
	}

	// Add to main thing
	container->addWidget(songs);
	return container;
}

QMenu *MainWindow::songMenu(const QString &trackId, const QString &artist,
	const QString &name, const QString &artistId, const QString &albumId)
{
	return new SongMenu(trackId, artist, name, artistId, albumId, *spotify, this);
}

void MainWindow::openAudioFeaturesWidget(const QString &trackId, const QString &artist, const QString &name)
{
	auto view = new AudioFeaturesView(*spotify, trackId, artist, name, this);
	if (audioFeaturesView != nullptr)
	{
		audioFeaturesView->close();
		audioFeaturesView->deleteLater();
	}
	audioFeaturesView = view;
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, audioFeaturesView);
}

void MainWindow::openLyrics(const QString &artist, const QString &name)
{
	auto view = new LyricsView(artist, name, this);
	if (!view->lyricsFound())
	{
		view->deleteLater();
		return;
	}
	if (lyricsView != nullptr)
	{
		lyricsView->close();
		lyricsView->deleteLater();
	}
	lyricsView = view;
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, lyricsView);
}

void MainWindow::refreshPlaylists()
{
	QListWidgetItem *currentItem = nullptr;
	QString lastItem;
	if (playlists->currentItem() != nullptr)
		lastItem = playlists->currentItem()->data(RolePlaylistId).toString();

	sptPlaylists = spotify->playlists();

	// Add all playlists
	playlists->clear();
	auto i = 0;
	QTextDocument doc;
	for (auto &playlist : sptPlaylists)
	{
		auto item = new QListWidgetItem(playlist.name, playlists);

		doc.setHtml(playlist.description);
		item->setToolTip(doc.toPlainText());

		item->setData(RolePlaylistId, playlist.id);
		item->setData(RoleIndex, i++);

		if (playlist.id == lastItem)
			currentItem = item;
	}

	// Sort
	if (settings.general.playlistOrder != PlaylistOrderDefault)
		orderPlaylists(settings.general.playlistOrder);

	if (currentItem != nullptr)
		playlists->setCurrentItem(currentItem);
}

bool MainWindow::loadSongs(const QVector<spt::Track> &tracks)
{
	songs->clear();
	trackItems.clear();
	playingTrackItem = nullptr;

	for (int i = 0; i < tracks.length(); i++)
	{
		auto track = tracks.at(i);
		auto item = new QTreeWidgetItem({
			settings.general.trackNumbers == ContextAll
				? QString("%1").arg(i + 1, 3)
				: "",
			track.name, track.artist, track.album,
			Utils::formatTime(track.duration),
			track.addedAt.date().toString(Qt::SystemLocaleShortDate)
		});
		item->setIcon(0, emptyIcon);
		item->setData(0, RoleTrackId, QString("spotify:track:%1").arg(track.id));
		item->setData(0, RoleArtistId, track.artistId);
		item->setData(0, RoleAlbumId, track.albumId);
		item->setData(0, RoleIndex, i);
		if (track.isLocal || !track.isPlayable)
		{
			item->setDisabled(true);
			item->setToolTip(1, track.isLocal
				? "Local track"
				: "Unavailable");
		}
		else if (track.id == current.item.id)
			setPlayingTrackItem(item);
		songs->insertTopLevelItem(i, item);
		trackItems[track.id] = item;
	}
	return true;
}

bool MainWindow::loadAlbum(const QString &albumId, bool ignoreEmpty)
{
	auto tracks = loadTracksFromCache(albumId);
	if (tracks.isEmpty())
		tracks = spotify->albumTracks(albumId);

	if (ignoreEmpty && tracks.length() <= 1)
		setStatus("Album only contains one song or is empty", true);
	else
	{
		playlists->setCurrentRow(-1);
		libraryList->setCurrentItem(nullptr);
		sptContext = QString("spotify:album:%1").arg(albumId);
		loadSongs(tracks);
		saveTracksToCache(albumId, tracks);
	}

	return true;
}

QVector<spt::Track> MainWindow::loadTracksFromCache(const QString &id)
{
	QVector<spt::Track> tracks;
	auto filePath = QString("%1/tracks/%2.json").arg(cacheLocation).arg(id);
	if (!QFileInfo::exists(filePath))
		return tracks;
	QFile file(filePath);
	file.open(QIODevice::ReadOnly);
	auto json = QJsonDocument::fromJson(file.readAll());
	file.close();
	if (json.isNull())
		return tracks;
	for (auto track : json.array())
		tracks.append(spt::Track(track.toObject()));
	return tracks;
}

void MainWindow::saveTracksToCache(const QString &id, const QVector<spt::Track> &tracks)
{
	QJsonArray json;
	for (auto &track : tracks)
		json.append(track.toJson());
	QFile file(QString("%1/tracks/%2.json").arg(cacheLocation).arg(id));
	file.open(QIODevice::WriteOnly);
	file.write(QJsonDocument(json).toJson());
}

bool MainWindow::loadPlaylist(spt::Playlist &playlist)
{
	if (!getPlaylistNameFromSaved(playlist.id).isEmpty())
	{
		settings.general.lastPlaylist = playlist.id;
		settings.save();
	}
	if (loadPlaylistFromCache(playlist))
		return true;
	songs->setEnabled(false);
	auto result = loadSongs(playlist.loadTracks(*spotify));
	songs->setEnabled(true);
	sptContext = QString("spotify:playlist:%1").arg(playlist.id);
	if (result)
		cachePlaylist(playlist);
	return result;
}

bool MainWindow::loadPlaylistFromCache(spt::Playlist &playlist)
{
	auto tracks = playlistTracks(playlist.id);
	if (tracks.isEmpty())
		return false;
	songs->setEnabled(false);
	auto result = loadSongs(tracks);
	songs->setEnabled(true);
	sptContext = QString("spotify:playlist:%1").arg(playlist.id);
	refreshPlaylist(playlist);
	return result;
}

QVector<spt::Track> MainWindow::playlistTracks(const QString &playlistId)
{
	QVector<spt::Track> tracks;
	auto filePath = QString("%1/playlist/%2.json").arg(cacheLocation).arg(playlistId);
	if (!QFileInfo::exists(filePath))
		return tracks;
	QFile file(filePath);
	file.open(QIODevice::ReadOnly);
	auto json = QJsonDocument::fromJson(file.readAll());
	file.close();
	if (json.isNull())
		return tracks;
	for (auto track : json.object()["tracks"].toArray())
		tracks.append(spt::Track(track.toObject()));
	return tracks;
}

void MainWindow::refreshPlaylist(spt::Playlist &playlist)
{
	auto newPlaylist = spotify->playlist(playlist.id);
	auto tracks = newPlaylist.loadTracks(*spotify);
	if (sptContext.endsWith(playlist.id))
		loadSongs(tracks);
	cachePlaylist(newPlaylist);
}

void MainWindow::setStatus(const QString &message, bool important)
{
	if (trayIcon != nullptr && settings.general.trayNotifications)
	{
		if (important)
			trayIcon->message(message);
	}
	else
		statusBar()->showMessage(message, 5000);
}

void MainWindow::setAlbumImage(const QString &url)
{
	nowAlbum->setPixmap(Utils::mask(getAlbum(url)));
}

QByteArray MainWindow::get(const QString &url)
{
	auto reply = network->get(QNetworkRequest(QUrl(url)));
	while (!reply->isFinished())
		QCoreApplication::processEvents();
	reply->deleteLater();
	return reply->error() == QNetworkReply::NoError
		? reply->readAll()
		: QByteArray();
}

QJsonDocument MainWindow::getJson(const QString &url)
{
	return QJsonDocument::fromJson(get(url));
}

QPixmap MainWindow::getImage(const QString &type, const QString &url)
{
	QPixmap img;
	if (url.isEmpty())
		return img;
	// Check if cache exists
	auto cachePath = QString("%1/%2/%3").arg(cacheLocation).arg(type).arg(QFileInfo(url).baseName());
	if (QFileInfo::exists(cachePath))
	{
		// Read file from cache
		QFile file(cachePath);
		file.open(QIODevice::ReadOnly);
		img.loadFromData(file.readAll(), "jpeg");
	}
	else
	{
		// Download image and save to cache
		img.loadFromData(get(url), "jpeg");
		if (!img.save(cachePath, "jpeg"))
			qDebug() << "failed to save album cache to" << cachePath;
	}
	return img;
}

QPixmap MainWindow::getAlbum(const QString &url)
{
	return getImage("album", url);
}

void MainWindow::openArtist(const QString &artistId)
{
	auto view = new ArtistView(*spotify, artistId, this);
	if (artistView != nullptr)
	{
		artistView->close();
		artistView->deleteLater();
	}
	artistView = view;
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, artistView);
}

void MainWindow::cachePlaylist(spt::Playlist &playlist)
{
	// Remove old format if needed before caching
	auto baseFile = QString("%1/playlist/%2").arg(cacheLocation).arg(playlist.id);
	if (QFileInfo::exists(baseFile))
		QFile(baseFile).remove();

	// Save new
	QJsonDocument json(playlist.toJson(*spotify));
	QFile file(QString("%1.json").arg(baseFile));
	file.open(QIODevice::WriteOnly);
	file.write(json.toJson());
}

QStringList MainWindow::currentTracks()
{
	QStringList tracks;
	tracks.reserve(songs->topLevelItemCount());
	for (int i = 0; i < songs->topLevelItemCount(); i++)
	{
		auto trackId = songs->topLevelItem(i)->data(0, RoleTrackId).toString();
		// spotify:track: = 14
		if (trackId.length() > 14)
			tracks.append(trackId);
	}
	return tracks;
}

void MainWindow::reloadTrayIcon()
{
	if (trayIcon != nullptr)
	{
		delete trayIcon;
		trayIcon = nullptr;
	}
	if (settings.general.trayIcon)
		trayIcon = new TrayIcon(spotify, settings, this);
}

void MainWindow::setPlayingTrackItem(QTreeWidgetItem *item)
{
	if (playingTrackItem != nullptr)
		playingTrackItem->setIcon(0, emptyIcon);

	if (item == nullptr)
	{
		playingTrackItem = nullptr;
		return;
	}
	item->setIcon(0, Icon::get("media-playback-start"));
	playingTrackItem = item;
}

QSet<QString> MainWindow::allArtists()
{
	QSet<QString> artists;
	for (auto i = 0; i < playlists->count(); i++)
		for (auto &track : playlistTracks(playlists->item(i)->data(RolePlaylistId).toString()))
			artists << track.artist;
	return artists;
}

void MainWindow::setFixedWidthTime(bool value)
{
	((MainToolBar *) toolBar)->position->setFont(value ? QFont("monospace") : QFont());
}

QIcon MainWindow::currentContextIcon() const
{
	return Icon::get(QString("view-media-%1")
		.arg(current.contextType.isEmpty()
			? "track"
			: current.contextType == "album"
				? "album-cover"
				: current.contextType));
}

void MainWindow::updateContextIcon()
{
	if (!settings.general.showContextInfo)
	{
		contextIcon->setVisible(false);
		contextInfo->setVisible(false);
		return;
	}

	auto currentName = current.contextType.isEmpty() || current.contextUri.isEmpty()
		? "No context"
		: current.contextType == "album"
			? current.item.album
			: current.contextType == "artist"
				? current.item.artist
				: getPlaylistName(current.contextUri);

	contextInfo->setText(currentName);
	auto size = contextInfo->fontInfo().pixelSize();
	contextIcon->setPixmap(currentContextIcon().pixmap(size, size));

	auto show = currentName != "No context";
	contextIcon->setVisible(show);
	contextInfo->setVisible(show);
}

QString MainWindow::getPlaylistNameFromSaved(const QString &id)
{
	for (auto &playlist : sptPlaylists)
	{
		if (id.endsWith(playlist.id))
			return playlist.name;
	}
	return QString();
}

QString MainWindow::getPlaylistName(const QString &id)
{
	auto name = getPlaylistNameFromSaved(id);
	if (!name.isEmpty())
		return name;
	return spotify->playlist(id.split(':').last()).name;
}

void MainWindow::contextInfoMenu(const QPoint &pos)
{
	auto menu = new QMenu(contextInfo);
	auto open = menu->addAction(currentContextIcon(),
		QString("Open %1").arg(current.contextType));
	menu->popup(contextInfo->mapToGlobal(pos));
	QAction::connect(open, &QAction::triggered, this, &MainWindow::contextInfoOpen);
}

void MainWindow::contextInfoOpen(bool)
{
	auto type = current.contextType;
	auto uri = current.contextUri.split(':').last();

	if (type == "album")
		loadAlbum(uri);
	else if (type == "artist")
		openArtist(uri);
	else if (type == "playlist")
	{
		auto playlist = spotify->playlist(uri);
		libraryList->setCurrentItem(nullptr);
		playlists->setCurrentRow(-1);
		loadPlaylist(playlist);
	}
}

int latestTrack(const QVector<spt::Track> &tracks)
{
	auto latest = 0;
	for (int i = 0; i < tracks.length(); i++)
	{
		if (tracks[i].addedAt > tracks[latest].addedAt)
			latest = i;
	}
	return latest;
}

void MainWindow::orderPlaylists(PlaylistOrder order)
{
	QList<QListWidgetItem *> items;
	items.reserve(playlists->count());

	auto i = 0;
	while (playlists->item(0) != nullptr)
		items.insert(i, playlists->takeItem(0));

	QMap<QString, int> customOrder;

	switch (order)
	{
		case PlaylistOrderDefault:
			std::sort(items.begin(), items.end(), [](QListWidgetItem *i1, QListWidgetItem *i2)
			{
				return i1->data(RoleIndex).toInt() < i2->data(RoleIndex).toInt();
			});
			break;

		case PlaylistOrderAlphabetical:
			std::sort(items.begin(), items.end(), [](QListWidgetItem *i1, QListWidgetItem *i2)
			{
				return i1->text() < i2->text();
			});
			break;

		case PlaylistOrderRecent:
			// TODO: Currently sorts by when tracks where added, not when playlist was last played
			std::sort(items.begin(), items.end(), [this](QListWidgetItem *i1, QListWidgetItem *i2)
			{
				auto t1 = playlistTracks(i1->data(DataRole::RolePlaylistId).toString());
				auto t2 = playlistTracks(i2->data(DataRole::RolePlaylistId).toString());

				return
					t1.length() > 0 && t2.length() > 0
						? t1.at(latestTrack(t1)).addedAt > t2.at(latestTrack(t2)).addedAt
						: false;
			});
			break;

		case PlaylistOrderCustom:
			i = 0;
			for (auto &playlist : settings.general.customPlaylistOrder)
				customOrder[playlist] = i++;
			std::sort(items.begin(), items.end(), [customOrder](QListWidgetItem *i1, QListWidgetItem *i2)
			{
				auto id1 = i1->data(DataRole::RolePlaylistId).toString();
				auto id2 = i2->data(DataRole::RolePlaylistId).toString();

				return
					customOrder.contains(id1) && customOrder.contains(id2)
						? customOrder[id1] < customOrder[id2]
						: false;
			});
			break;
	}

	for (auto item : items)
		playlists->addItem(item);
}

void MainWindow::toggleTrackNumbers(bool enabled)
{
	for (int i = 0; i < songs->topLevelItemCount(); i++)
	{
		auto item = songs->topLevelItem(i);
		item->setText(0, enabled
			? QString("%1").arg(item->data(0, RoleIndex).toInt() + 1, 3)
			: QString());
	}
}

//region Getters

spt::Playback MainWindow::currentPlayback()
{
	return current;
}

bool MainWindow::hasPlaylistSelected()
{
	return playlists->currentRow() >= 0;
}

spt::User MainWindow::getCurrentUser()
{
	return currentUser;
}

QString &MainWindow::getCacheLocation()
{
	return cacheLocation;
}

QVector<spt::Playlist> &MainWindow::getSptPlaylists()
{
	return sptPlaylists;
}

QListWidget *MainWindow::getPlaylistsList()
{
	return playlists;
}

QAction *MainWindow::getSearchAction()
{
	return ((MainToolBar *) toolBar)->search;
}

QTreeWidget *MainWindow::getSongsTree()
{
	return songs;
}

LibraryList *MainWindow::getLibraryList()
{
	return libraryList;
}

QString &MainWindow::getSptContext()
{
	return sptContext;
}

spt::Playback &MainWindow::getCurrentPlayback()
{
	return current;
}

mp::Service *MainWindow::getMediaPlayer()
{
	return mediaPlayer;
}

bool MainWindow::isValid() const
{
	return stateValid;
}

//endregion