#include "mainwindow.hpp"

MainWindow::MainWindow(Settings &settings)
	: settings(settings), QMainWindow()
{
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
	spotify = new spt::Spotify(settings, this);
	network = new QNetworkAccessManager();

	// Setup main window
	setWindowTitle("spotify-qt");
	setWindowIcon(Icon::get("logo:spotify-qt"));
	resize(1280, 720);
	setCentralWidget(createCentralWidget());
	addToolBar(Qt::ToolBarArea::TopToolBarArea, createToolBar());

	// Update player status
	auto timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout, this, &MainWindow::refresh);
	refresh();
	timer->start(1000);

	// Check if should start client
	if (settings.spotify.startClient
		&& (settings.spotify.alwaysStart || spotify->devices().isEmpty()))
	{
		sptClient = new spt::ClientHandler(settings, this);
		auto status = sptClient->start();
		if (!status.isEmpty())
			QMessageBox::warning(
				this,
				"Client error",
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
	if (!current.isPlaying && current.item.name == "(no name)")
	{
		playPause->setIcon(Icon::get("media-playback-start"));
		playPause->setText("Play");
		return;
	}
	auto currPlaying = QString("%1\n%2").arg(current.item.name).arg(current.item.artist);
	if (nowPlaying->text() != currPlaying)
	{
		if (current.isPlaying)
			setPlayingTrackItem(trackItems.contains(current.item.id)
				? trackItems[current.item.id] : nullptr);
		nowPlaying->setText(currPlaying);
		setAlbumImage(current.item.image);
		setWindowTitle(QString("%1 - %2").arg(current.item.artist).arg(current.item.name));
		if (mediaPlayer != nullptr)
			mediaPlayer->currentSourceChanged(current);
		if (trayIcon != nullptr && settings.general.trayAlbumArt)
			trayIcon->setPixmap(getAlbum(current.item.image));
	}
	position->setText(QString("%1/%2")
		.arg(Utils::formatTime(current.progressMs))
		.arg(Utils::formatTime(current.item.duration)));
	progress->setValue(current.progressMs);
	progress->setMaximum(current.item.duration);
	playPause->setIcon(Icon::get(
		current.isPlaying ? "media-playback-pause" : "media-playback-start"));
	playPause->setText(current.isPlaying ? "Pause" : "Play");
	if (!settings.general.pulseVolume)
		volumeButton->setVolume(current.volume / 5);
	repeat->setChecked(current.repeat != "off");
	shuffle->setChecked(current.shuffle);
}

QWidget *MainWindow::createCentralWidget()
{
	auto container = new QSplitter();
	auto sidebar = new QVBoxLayout();

	//region Library
	libraryList = new LibraryList(*spotify, this);
	auto library = Utils::createGroupBox(QVector<QWidget*>() << libraryList, this);
	library->setTitle("Library");
	sidebar->addWidget(library);
	//endregion

	//region Playlists
	playlists = new PlaylistList(*spotify, this);
	refreshPlaylists();

	auto playlistContainer = Utils::createGroupBox(QVector<QWidget*>() << playlists, this);
	playlistContainer->setTitle("Playlists");
	sidebar->addWidget(playlistContainer);
	//endregion

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
	QLabel::connect(nowPlaying, &QWidget::customContextMenuRequested, [this](const QPoint &pos) {
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
		playlistId = sptPlaylists.at(0).id;
	}
	else
	{
		// Find playlist in list
		int i = 0;
		for (auto &playlist : sptPlaylists)
		{
			if (playlist.id == playlistId)
			{
				playlists->setCurrentRow(i);
				loadPlaylist(playlist);
			}
			i++;
		}
	}
	// Add to main thing
	container->addWidget(songs);
	return container;
}

QMenu *MainWindow::songMenu(const QString &trackId, const QString &artist,
	const QString &name, const QString &artistId, const QString &albumId)
{
	return new SongMenu(trackId, artist, name, artistId, albumId, spotify, this);
}

QToolBar *MainWindow::createToolBar()
{
	auto toolBar = new QToolBar("Media controls", this);
	toolBar->setMovable(false);
	// Menu
	auto menu = new QToolButton(this);
	menu->setText("Menu");
	menu->setIcon(Icon::get("application-menu"));
	menu->setPopupMode(QToolButton::InstantPopup);
	menu->setMenu(new MainMenu(*spotify, settings, this));
	toolBar->addWidget(menu);
	// Search
	search = toolBar->addAction(Icon::get("edit-find"), "Search");
	search->setCheckable(true);
	searchView = new SearchView(*spotify, this);
	addDockWidget(Qt::RightDockWidgetArea, searchView);
	searchView->hide();
	QAction::connect(search, &QAction::triggered, [this](bool checked) {
		searchView->setHidden(!checked);
	});
	// Media controls
	toolBar->addSeparator();
	auto previous = toolBar->addAction(Icon::get("media-skip-backward"), "Previous");
	playPause = toolBar->addAction(Icon::get("media-playback-start"), "Play");
	QAction::connect(playPause, &QAction::triggered, [=](bool checked) {
		current.isPlaying = !current.isPlaying;
		refreshed(current);
		auto status = playPause->iconText() == "Play" ? spotify->pause() : spotify->resume();
		if (!status.isEmpty())
		{
			setStatus(QString("Failed to %1 playback: %2")
				.arg(playPause->iconText() == "Pause" ? "pause" : "resume")
				.arg(status), true);
		}
	});
	auto next = toolBar->addAction(Icon::get("media-skip-forward"),  "Next");
	QAction::connect(previous, &QAction::triggered, [=](bool checked) {
		auto status = spotify->previous();
		if (!status.isEmpty())
			setStatus(QString("Failed to go to previous track: %1").arg(status), true);
		refresh();
	});
	QAction::connect(next, &QAction::triggered, [=](bool checked) {
		auto status = spotify->next();
		if (!status.isEmpty())
			setStatus(QString("Failed to go to next track: %1").arg(status), true);
		refresh();
	});
	// Progress
	progress = new QSlider(this);
	progress->setOrientation(Qt::Orientation::Horizontal);
	QSlider::connect(progress, &QAbstractSlider::sliderReleased, [=]() {
		auto status = spotify->seek(progress->value());
		if (!status.isEmpty())
			setStatus(QString("Failed to seek: %1").arg(status), true);
		if (mediaPlayer != nullptr)
			mediaPlayer->stateUpdated();
	});
	toolBar->addSeparator();
	toolBar->addWidget(progress);
	toolBar->addSeparator();
	position = new QLabel("0:00/0:00", this);
	if (settings.general.fixedWidthTime)
		position->setFont(QFont("monospace"));
	toolBar->addWidget(position);
	toolBar->addSeparator();
	// Shuffle and repeat toggles
	shuffle = toolBar->addAction(Icon::get("media-playlist-shuffle"), "Shuffle");
	shuffle->setCheckable(true);
	QAction::connect(shuffle, &QAction::triggered, [=](bool checked) {
		current.shuffle = !current.shuffle;
		refreshed(current);
		auto status = spotify->setShuffle(checked);
		if (!status.isEmpty())
			setStatus(QString("Failed to toggle shuffle: %1").arg(status), true);
	});
	repeat = toolBar->addAction(Icon::get("media-playlist-repeat"), "Repeat");
	repeat->setCheckable(true);
	QAction::connect(repeat, &QAction::triggered, [=](bool checked) {
		auto repeatMode = QString(checked ? "context" : "off");
		current.repeat = repeatMode;
		refreshed(current);
		auto status = spotify->setRepeat(repeatMode);
		if (!status.isEmpty())
			setStatus(QString("Failed to toggle repeat: %1").arg(status), true);
	});
	// Volume
	volumeButton = new VolumeButton(settings, *spotify, this);
	toolBar->addWidget(volumeButton);
	// Return final tool bar
	return toolBar;
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
	auto lastIndex =
		playlists == nullptr
		? -1
		: playlists->currentRow();
	sptPlaylists = spotify->playlists();

	// Add all playlists
	playlists->clear();
	for (auto &playlist : sptPlaylists)
	{
		auto item = new QListWidgetItem(playlist.name, playlists);
		item->setToolTip(playlist.description);
		item->setData(Utils::RolePlaylistId, playlist.id);
	}
	if (lastIndex >= 0)
		playlists->setCurrentRow(lastIndex);
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
			"", track.name, track.artist, track.album,
			Utils::formatTime(track.duration), track.addedAt.date().toString(Qt::SystemLocaleShortDate)
		});
		item->setData(0, Utils::RoleTrackId,  QString("spotify:track:%1").arg(track.id));
		item->setData(0, Utils::RoleArtistId, track.artistId);
		item->setData(0, Utils::RoleAlbumId,  track.albumId);
		item->setData(0, Utils::RoleIndex,    i);
		if (track.isLocal)
		{
			item->setDisabled(true);
			item->setToolTip(1, "Local track");
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
	settings.general.lastPlaylist = playlist.id;
	settings.save();
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
	return reply->readAll();
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
	auto view = new ArtistView(spotify, artistId, this);
	if (artistView != nullptr)
	{
		artistView->close();
		artistView->deleteLater();
	}
	artistView = view;
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, artistView);
	libraryList->setCurrentItem(nullptr);
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
		auto trackId = songs->topLevelItem(i)->data(0, Utils::RoleTrackId).toString();
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
		playingTrackItem->setIcon(0, QIcon());
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
		for (auto &track : playlistTracks(playlists->item(i)->data(Utils::RolePlaylistId).toString()))
			artists << track.artist;
	return artists;
}

void MainWindow::setFixedWidthTime(bool value)
{
	position->setFont(value ? QFont("monospace") : QFont());
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
	return search;
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

//endregion