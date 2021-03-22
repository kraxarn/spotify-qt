#include "mainwindow.hpp"

MainWindow::MainWindow(lib::settings &settings, lib::paths &paths)
	: settings(settings),
	cache(paths),
	QMainWindow()
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
	stateValid = spotify->refresh();
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
	if (settings.spotify.start_client)
	{
		auto startClient = [this]()
		{
			this->sptClient = new spt::ClientHandler(this->settings, this);
			auto status = sptClient->start();
			if (!status.isEmpty())
			{
				QMessageBox::warning(this, "Client error",
					QString("Failed to autostart Spotify client: %1").arg(status));
			}
		};
		if (settings.spotify.always_start)
			startClient();
		else
		{
			spotify->devices([startClient](const std::vector<lib::spt::device> &devices)
			{
				if (devices.empty())
					startClient();
			});
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
	{
		auto dialog = new WhatsNewDialog(APP_VERSION, settings, this);

		if (dialog->isValid())
			dialog->open();
		else
			lib::log::error("Failed to fetch what's new in \"{}\"", APP_VERSION);
	}

	settings.general.last_version = APP_VERSION;
	settings.save();

	// Get current user
	spotify->me([this](const spt::User &user)
	{
		this->currentUser = user;
	});

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
		|| current.playback.progress_ms + 1000 > current.playback.item.duration)
	{
		spotify->currentPlayback([this](const lib::spt::playback &playback)
		{
			refreshed(playback);
		});
		refreshCount = 0;
		return;
	}

	// Assume last refresh was 1 sec ago
	if (!current.playback.is_playing)
		return;
	current.playback.progress_ms += 1000;
	refreshed(current.playback);
}

void MainWindow::refreshed(const lib::spt::playback &playback)
{
	current.playback = playback;

	auto mainToolBar = dynamic_cast<MainToolBar *>(toolBar);

	if (!current.playback.is_playing && current.playback.item.name == "(no name)")
	{
		mainToolBar->playPause->setIcon(Icon::get("media-playback-start"));
		mainToolBar->playPause->setText("Play");
		return;
	}
	auto currPlaying = QString::fromStdString(lib::fmt::format("{}\n{}",
		current.playback.item.name, current.playback.item.artist));
	if (leftSidePanel->getCurrentlyPlaying() != currPlaying)
	{
		if (current.playback.is_playing)
		{
			auto itemId = current.playback.item.id;
			setPlayingTrackItem(trackItems.find(itemId) != trackItems.end()
				? trackItems[itemId]
				: nullptr);
		}

		leftSidePanel->setCurrentlyPlaying(currPlaying);
		setAlbumImage(QString::fromStdString(current.playback.item.image));
		setWindowTitle(QString::fromStdString(current.playback.item.title()));
		leftSidePanel->updateContextIcon();

#ifdef USE_DBUS
		if (mediaPlayer != nullptr)
			mediaPlayer->currentSourceChanged(current.playback);
#endif

		if (trayIcon != nullptr && settings.general.tray_album_art)
			trayIcon->setPixmap(getAlbum(current.playback.item.image));
	}

	mainToolBar->position->setText(QString("%1/%2")
		.arg(QString::fromStdString(lib::fmt::time(current.playback.progress_ms)))
		.arg(QString::fromStdString(lib::fmt::time(current.playback.item.duration))));

	mainToolBar->progress->setValue(current.playback.progress_ms);
	mainToolBar->progress->setMaximum(current.playback.item.duration);
	mainToolBar->playPause->setIcon(Icon::get(current.playback.is_playing
		? "media-playback-pause"
		: "media-playback-start"));
	mainToolBar->playPause->setText(current.playback.is_playing ? "Pause" : "Play");

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

void MainWindow::openAudioFeaturesWidget(const std::string &trackId,
	const std::string &artist, const std::string &name)
{
	dynamic_cast<SidePanel *>(sidePanel)->openAudioFeatures(trackId, artist, name);
}

void MainWindow::openLyrics(const std::string &artist, const std::string &name)
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

bool MainWindow::loadSongs(const std::vector<lib::spt::track> &tracks,
	const std::string &selectedId)
{
	songs->clear();
	trackItems.clear();
	playingTrackItem = nullptr;
	auto fieldWidth = QString::number(tracks.size()).length();

	for (int i = 0; i < tracks.size(); i++)
	{
		const auto &track = tracks.at(i);
		auto item = new TrackListItem({
			settings.general.track_numbers == lib::context_all
				? QString("%1").arg(i + 1, fieldWidth)
				: "",
			QString::fromStdString(track.name),
			QString::fromStdString(track.artist),
			QString::fromStdString(track.album),
			QString::fromStdString(lib::fmt::time(track.duration)),
			track.added_at.empty()
				? QString()
				: settings.general.relative_added
				? DateUtils::toRelative(track.added_at)
				: QLocale().toString(DateUtils::fromIso(track.added_at).date(),
					QLocale::ShortFormat)
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

bool MainWindow::loadAlbum(const std::string &albumId, const std::string &trackId)
{
	auto tracks = loadTracksFromCache(albumId);
	if (tracks.empty())
		tracks = spotify->albumTracks(albumId);

	if (tracks.empty())
		setStatus(QString("Album is empty"), true);
	else
	{
		leftSidePanel->setCurrentPlaylistItem(-1);
		leftSidePanel->setCurrentLibraryItem(nullptr);
		current.context = QString::fromStdString(lib::fmt::format(
			"spotify:album:{}", albumId));
		loadSongs(tracks, trackId);
		saveTracksToCache(albumId, tracks);
	}

	return true;
}

std::vector<lib::spt::track> MainWindow::loadTracksFromCache(const std::string &id)
{
	return cache.tracks(id);
}

void MainWindow::saveTracksToCache(const std::string &id,
	const std::vector<lib::spt::track> &tracks)
{
	cache.tracks(id, tracks);
}

bool MainWindow::loadPlaylist(const spt::Playlist &playlist)
{
	if (!leftSidePanel->getPlaylistNameFromSaved(playlist.id.toStdString()).isEmpty())
	{
		settings.general.last_playlist = playlist.id.toStdString();
		settings.save();
	}
	if (loadPlaylistFromCache(playlist))
		return true;
	songs->setEnabled(false);
	std::vector<lib::spt::track> tracks;
	auto result = playlist.loadTracks(*spotify, tracks);
	if (result)
		loadSongs(tracks);
	songs->setEnabled(true);
	current.context = QString("spotify:playlist:%1").arg(playlist.id);
	if (result)
		cachePlaylist(playlist);
	return result;
}

bool MainWindow::loadPlaylistFromCache(const spt::Playlist &playlist)
{
	auto tracks = playlistTracks(playlist.id.toStdString());
	if (tracks.empty())
		return false;
	songs->setEnabled(false);
	auto result = loadSongs(tracks);
	songs->setEnabled(true);
	current.context = QString("spotify:playlist:%1").arg(playlist.id);
	refreshPlaylist(playlist);
	return result;
}

std::vector<lib::spt::track> MainWindow::playlistTracks(const std::string &playlistId)
{
	return cache.get_playlist_tracks(playlistId);
}

void MainWindow::refreshPlaylist(const spt::Playlist &playlist)
{
	auto newPlaylist = spotify->playlist(playlist.id.toStdString());
	std::vector<lib::spt::track> tracks;
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

void MainWindow::status(const std::string &message, bool important)
{
	setStatus(QString::fromStdString(message), important);
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

QPixmap MainWindow::getAlbum(const std::string &url)
{
	return getAlbum(QString::fromStdString(url));
}

void MainWindow::openArtist(const std::string &artistId)
{
	dynamic_cast<SidePanel *>(sidePanel)->openArtist(artistId);
}

void MainWindow::cachePlaylist(const spt::Playlist &playlist)
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

std::vector<std::string> MainWindow::currentTracks()
{
	std::vector<std::string> tracks;
	for (int i = 0; i < songs->topLevelItemCount(); i++)
	{
		auto trackId = songs->topLevelItem(i)->data(0, RoleTrackId)
			.toString().toStdString();
		// spotify:track: = 14
		if (trackId.length() > 14)
			tracks.push_back(trackId);
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

lib::spt::playback MainWindow::currentPlayback() const
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

std::string MainWindow::getSptContext() const
{
	return current.context.toStdString();
}

lib::spt::playback &MainWindow::getCurrentPlayback()
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

std::unordered_set<std::string> MainWindow::allArtists()
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