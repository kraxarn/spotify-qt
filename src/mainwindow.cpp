#include "mainwindow.hpp"

MainWindow::MainWindow(lib::settings &settings)
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
	QApplication::setStyle(QString::fromStdString(settings.general.style));
	Utils::applyPalette(settings.general.style_palette);

	// Custom dark theme
	if (settings.general.style_palette == lib::palette_dark)
	{
		QFile styleFile(":/res/style/dark.qss");
		styleFile.open(QFile::ReadOnly | QFile::Text);
		setStyleSheet(QString::fromUtf8(styleFile.readAll()));
		styleFile.close();
	}

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
	if (settings.spotify.start_client
		&& (settings.spotify.always_start || spotify->devices().isEmpty()))
	{
		sptClient = new spt::ClientHandler(settings, this);
		auto status = sptClient->start();
		if (!status.isEmpty())
		{
			QMessageBox::warning(this, "Client error",
				QString("Failed to autostart Spotify client: %1").arg(status));
		}
	}

	// Start media controller if specified
#ifdef USE_DBUS
	if (settings.general.media_controller)
	{
		mediaPlayer = new mp::Service(spotify, this);
		// Check if something went wrong during init
		if (!mediaPlayer->isValid())
		{
			delete mediaPlayer;
			mediaPlayer = nullptr;
		}
	}
#endif

	// Create tray icon if specified
	if (settings.general.tray_icon)
		trayIcon = new TrayIcon(spotify, settings, this);

	// If new version has been detected, show what's new dialog
	if (settings.general.show_changelog
		&& settings.general.last_version != APP_VERSION)
		(new WhatsNewDialog(APP_VERSION, settings, this))->open();
	settings.general.last_version = APP_VERSION;
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
		|| ++refreshCount >= settings.general.refresh_interval
		|| current.playback.progressMs + 1000 > current.playback.item.duration)
	{
		spotify->currentPlayback([this](const spt::Playback &playback)
		{
			refreshed(playback);
		});
		refreshCount = 0;
		return;
	}

	// Assume last refresh was 1 sec ago
	if (!current.playback.isPlaying)
		return;
	current.playback.progressMs += 1000;
	refreshed(current.playback);
}

void MainWindow::refreshed(const spt::Playback &playback)
{
	// Just so it doesn't make the app startup slower for now
	if (currentUser.id.isEmpty())
		currentUser = spotify->me();
	current.playback = playback;

	auto mainToolBar = dynamic_cast<MainToolBar *>(toolBar);

	if (!current.playback.isPlaying && current.playback.item.name == "(no name)")
	{
		mainToolBar->playPause->setIcon(Icon::get("media-playback-start"));
		mainToolBar->playPause->setText("Play");
		return;
	}
	auto currPlaying = QString("%1\n%2")
		.arg(current.playback.item.name)
		.arg(current.playback.item.artist);
	if (leftSidePanel->getCurrentlyPlaying() != currPlaying)
	{
		if (current.playback.isPlaying)
		{
			setPlayingTrackItem(trackItems.contains(current.playback.item.id)
				? trackItems[current.playback.item.id]
				: nullptr);
		}

		leftSidePanel->setCurrentlyPlaying(currPlaying);
		setAlbumImage(current.playback.item.image);
		setWindowTitle(QString("%1 - %2")
			.arg(current.playback.item.artist)
			.arg(current.playback.item.name));
		leftSidePanel->updateContextIcon();

#ifdef USE_DBUS
		if (mediaPlayer != nullptr)
			mediaPlayer->currentSourceChanged(current.playback);
#endif

		if (trayIcon != nullptr && settings.general.tray_album_art)
			trayIcon->setPixmap(getAlbum(current.playback.item.image));
	}
	mainToolBar->position->setText(QString("%1/%2")
		.arg(Utils::formatTime(current.playback.progressMs))
		.arg(Utils::formatTime(current.playback.item.duration)));
	mainToolBar->progress->setValue(current.playback.progressMs);
	mainToolBar->progress->setMaximum(current.playback.item.duration);
	mainToolBar->playPause->setIcon(Icon::get(current.playback.isPlaying
		? "media-playback-pause"
		: "media-playback-start"));
	mainToolBar->playPause->setText(current.playback.isPlaying ? "Pause" : "Play");
	if (!settings.general.pulse_volume)
		mainToolBar->volumeButton->setVolume(current.playback.volume() / 5);
	mainToolBar->repeat->setChecked(current.playback.repeat != "off");
	mainToolBar->shuffle->setChecked(current.playback.shuffle);
}

QWidget *MainWindow::createCentralWidget()
{
	auto container = new QSplitter();

	// Left side panel with library and playlists
	leftSidePanel = new LeftSidePanel(*spotify, settings, current, this);
	container->addWidget(leftSidePanel);

	//region Songs
	songs = new TracksList(*spotify, settings, this);
	//endregion

	// Load tracks in playlist
	auto playlistId = QString::fromStdString(settings.general.last_playlist);
	if (leftSidePanel->playlistCount() <= 0)
	{
		// If no playlists were found
		// TODO: Load something from library here
	}
	else if (playlistId.isEmpty())
	{
		// Default to first in list
		playlistId = leftSidePanel
			->playlistItem(0)
			->data(RolePlaylistId).toString();
	}

	// Find playlist in list
	for (auto i = 0; i < leftSidePanel->playlistCount(); i++)
	{
		auto item = leftSidePanel->playlistItem(i);

		if (item->data(RolePlaylistId).toString().endsWith(playlistId))
		{
			leftSidePanel->setCurrentPlaylistItem(i);
			loadPlaylist(leftSidePanel->playlist(item->data(RoleIndex).toInt()));
		}
	}

	// Add to main thing
	container->addWidget(songs);

	// Side panel
	sidePanel = new SidePanel(*spotify, settings, this);
	container->addWidget(sidePanel);

	return container;
}

void MainWindow::openAudioFeaturesWidget(const QString &trackId,
	const QString &artist, const QString &name)
{
	dynamic_cast<SidePanel *>(sidePanel)->openAudioFeatures(trackId, artist, name);
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

bool MainWindow::loadSongs(const QVector<spt::Track> &tracks, const QString &selectedId)
{
	songs->clear();
	trackItems.clear();
	playingTrackItem = nullptr;
	auto fieldWidth = QString::number(tracks.length()).length();

	for (int i = 0; i < tracks.length(); i++)
	{
		auto track = tracks.at(i);
		auto item = new TrackListItem({
			settings.general.track_numbers == lib::context_all
				? QString("%1").arg(i + 1, fieldWidth)
				: "",
			track.name, track.artist, track.album,
			Utils::formatTime(track.duration),
			DateUtils::isEmpty(track.addedAt)
				? QString()
				: settings.general.relative_added
				? DateUtils::toRelative(track.addedAt)
				: QLocale().toString(track.addedAt.date(), QLocale::ShortFormat)
		}, track, emptyIcon, i);

		if (track.id == current.playback.item.id)
			setPlayingTrackItem(item);

		songs->insertTopLevelItem(i, item);
		trackItems[track.id] = item;

		if (track.id == selectedId)
			songs->setCurrentItem(item);
	}

	return true;
}

bool MainWindow::loadAlbum(const QString &albumId, const QString &trackId)
{
	auto tracks = loadTracksFromCache(albumId);
	if (tracks.isEmpty())
		tracks = spotify->albumTracks(albumId);

	if (tracks.length() <= 0)
		setStatus("Album is empty", true);
	else
	{
		leftSidePanel->setCurrentPlaylistItem(-1);
		leftSidePanel->setCurrentLibraryItem(nullptr);
		current.context = QString("spotify:album:%1").arg(albumId);
		loadSongs(tracks, trackId);
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
	if (!leftSidePanel->getPlaylistNameFromSaved(playlist.id).isEmpty())
	{
		settings.general.last_playlist = playlist.id.toStdString();
		settings.save();
	}
	if (loadPlaylistFromCache(playlist))
		return true;
	songs->setEnabled(false);
	QVector<spt::Track> tracks;
	auto result = playlist.loadTracks(*spotify, tracks);
	if (result)
		loadSongs(tracks);
	songs->setEnabled(true);
	current.context = QString("spotify:playlist:%1").arg(playlist.id);
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
	current.context = QString("spotify:playlist:%1").arg(playlist.id);
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
	QVector<spt::Track> tracks;
	auto result = newPlaylist.loadTracks(*spotify, tracks);
	if (!result)
	{
		lib::log::error("Failed to refresh playlist \"{}\" ({})",
			playlist.name.toStdString(), playlist.id.toStdString());
		return;
	}
	if (current.context.endsWith(playlist.id))
		loadSongs(tracks);
	cachePlaylist(newPlaylist);
}

void MainWindow::setStatus(const QString &message, bool important)
{
	if (message.isNull() || message.isEmpty())
		return;

	if (trayIcon != nullptr && settings.general.tray_notifications)
	{
		if (important)
			trayIcon->message(message);
	}
	else
		statusBar()->showMessage(message, 5000);
}

void MainWindow::setAlbumImage(const QString &url)
{
	leftSidePanel->setAlbumImage(Utils::mask(getAlbum(url)));
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
	auto cachePath = QString("%1/%2/%3")
		.arg(cacheLocation)
		.arg(type)
		.arg(QFileInfo(url).baseName());
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
			lib::log::error("Failed to save album cache to {}", cachePath.toStdString());
	}
	return img;
}

QPixmap MainWindow::getAlbum(const QString &url)
{
	return getImage("album", url);
}

void MainWindow::openArtist(const QString &artistId)
{
	dynamic_cast<SidePanel *>(sidePanel)->openArtist(artistId);
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
	if (settings.general.tray_icon)
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

void MainWindow::setFixedWidthTime(bool value)
{
	((MainToolBar *) toolBar)->position->setFont(value ? QFont("monospace") : QFont());
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

spt::Playback MainWindow::currentPlayback() const
{
	return current.playback;
}

spt::User MainWindow::getCurrentUser()
{
	return currentUser;
}

QString &MainWindow::getCacheLocation()
{
	return cacheLocation;
}

QAction *MainWindow::getSearchAction()
{
	return ((MainToolBar *) toolBar)->search;
}

QTreeWidget *MainWindow::getSongsTree()
{
	return songs;
}

QString &MainWindow::getSptContext()
{
	return current.context;
}

spt::Playback &MainWindow::getCurrentPlayback()
{
	return current.playback;
}

#ifdef USE_DBUS
mp::Service *MainWindow::getMediaPlayer()
{
	return mediaPlayer;
}
#endif

bool MainWindow::isValid() const
{
	return stateValid;
}

//endregion

void MainWindow::setSearchVisible(bool visible)
{
	auto panel = dynamic_cast<SidePanel *>(sidePanel);
	if (visible)
		panel->openSearch();
	else
		panel->closeSearch();
}

void MainWindow::addSidePanelTab(QWidget *widget, const QString &title)
{
	sidePanel->addTab(widget, title);
	sidePanel->setCurrentWidget(widget);
	sidePanel->show();
}

void MainWindow::refreshPlaylists()
{
	leftSidePanel->refreshPlaylists();
}

void MainWindow::setCurrentLibraryItem(QTreeWidgetItem *item)
{
	leftSidePanel->setCurrentLibraryItem(item);
}

QTreeWidgetItem *MainWindow::getCurrentLibraryItem()
{
	return leftSidePanel->getCurrentLibraryItem();
}

spt::Playlist &MainWindow::getPlaylist(int index)
{
	return leftSidePanel->playlist(index);
}

void MainWindow::setCurrentPlaylistItem(int index)
{
	leftSidePanel->setCurrentPlaylistItem(index);
}

QSet<QString> MainWindow::allArtists()
{
	return leftSidePanel->allArtists();
}

QListWidgetItem *MainWindow::getCurrentPlaylistItem()
{
	return leftSidePanel->currentPlaylist();
}

QVector<spt::Playlist> &MainWindow::getPlaylists()
{
	return leftSidePanel->getPlaylists();
}

int MainWindow::getPlaylistItemCount()
{
	return leftSidePanel->playlistItemCount();
}

QListWidgetItem *MainWindow::getPlaylistItem(int index)
{
	return leftSidePanel->playlistItem(index);
}

void MainWindow::orderPlaylists(lib::playlist_order order)
{
	leftSidePanel->orderPlaylists(order);
}

MainWindow *MainWindow::find(QWidget *from)
{
	auto w = from;
	while (w != nullptr && typeid(*w) != typeid(MainWindow))
		w = w->parentWidget();
	return dynamic_cast<MainWindow *>(w);
}