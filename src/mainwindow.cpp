#include "mainwindow.hpp"

MainWindow::MainWindow(lib::settings &settings, lib::paths &paths)
	: settings(settings),
	paths(paths),
	cache(paths)
{
	lib::crash_handler::set_cache(cache);

	// Splash
	SplashDialog splash;
	splash.show();
	splash.showMessage("Please wait...");

	// Apply selected style and palette
	QApplication::setStyle(QString::fromStdString(settings.general.style));
	StyleUtils::applyPalette(settings.general.style_palette);

	// Custom dark theme
	if (settings.general.style_palette == lib::palette_dark)
	{
		setStyleSheet(DarkPalette::getDarkStylesheet());
	}

	// Check for dark background
	StyleUtils::setDarkBackground(this);

	// Set Spotify
	splash.showMessage("Connecting...");
	httpClient = new lib::qt::http_client(this);
	spotify = new spt::Spotify(settings, *httpClient, this);
	network = new QNetworkAccessManager(this);

	// Check connection
	stateValid = spotify->tryRefresh();
	if (!stateValid)
	{
		splash.finish(this);
		return;
	}

	// Setup main window
	setWindowTitle("spotify-qt");
	setWindowIcon(Icon::get("logo:spotify-qt"));
	resize(defaultSize());
	setCentralWidget(createCentralWidget());
	toolBar = new MainToolBar(*spotify, settings,
		*httpClient, cache, this);
	addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);

	// Update player status
	splash.showMessage("Refreshing...");
	auto *timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout, this, &MainWindow::refresh);
	refresh();
	constexpr int tickMs = 1000;
	timer->start(tickMs);
	splash.showMessage("Welcome!");

	// Start client if set
	initClient();

	// Start media controller if specified
	initMediaController();

	// Create tray icon if specified
	if (settings.general.tray_icon)
	{
		trayIcon = new TrayIcon(spotify, settings, this);
	}

	// If new version has been detected, show what's new dialog
	initWhatsNew();

	// Get current user
	spotify->me([this](const lib::spt::user &user)
	{
		this->currentUser = user;
	});

	initDevice();
	setBorderless(!settings.qt_const().system_title_bar);

	// Welcome
	setStatus("Welcome to spotify-qt!");
	splash.finish(this);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	delete trayIcon;
	event->accept();
}

void MainWindow::initClient()
{
	if (!settings.spotify.start_client)
	{
		return;
	}

	if (settings.spotify.always_start)
	{
		startClient();
	}
	else
	{
		spotify->devices([this](const std::vector<lib::spt::device> &devices)
		{
			if (devices.empty())
			{
				this->startClient();
			}
		});
	}
}

void MainWindow::initMediaController()
{
#ifdef USE_DBUS
	if (!settings.general.media_controller)
	{
		return;
	}

	mediaPlayer = new mp::Service(spotify, this);
	// Check if something went wrong during init
	if (!mediaPlayer->isValid())
	{
		delete mediaPlayer;
		mediaPlayer = nullptr;
	}
#endif
}

void MainWindow::initWhatsNew()
{
	if (settings.general.show_changelog
		&& !settings.general.last_version.empty()
		&& settings.general.last_version != APP_VERSION)
	{
		auto *dialog = new WhatsNewDialog(settings, *httpClient, this);
		dialog->open();
	}

	settings.general.last_version = APP_VERSION;
	settings.save();
}

void MainWindow::initDevice()
{
	spotify->devices([this](const std::vector<lib::spt::device> &devices)
	{
		// Don't select a new device if one is currently active
		for (const auto &device : devices)
		{
			if (device.is_active)
			{
				return;
			}
		}

		if (devices.size() == 1
			&& lib::strings::starts_with(devices.front().name, "spotify-qt"))
		{
			spotify->set_device(devices.front(), {});
		}
		else
		{
			for (const auto &device : devices)
			{
				if (device.id == this->settings.general.last_device)
				{
					spotify->set_device(device, {});
					break;
				}
			}
		}
	});
}

void MainWindow::setBorderless(bool enabled)
{
	setWindowFlag(Qt::FramelessWindowHint, enabled);

	if (enabled)
	{
		addSizeGrip([](const QRect &rect) -> QPoint
		{
			return rect.bottomLeft();
		});

		addSizeGrip([](const QRect &rect) -> QPoint
		{
			return rect.bottomRight();
		});

		addSizeGrip([](const QRect &rect) -> QPoint
		{
			return rect.topLeft();
		});

		addSizeGrip([](const QRect &rect) -> QPoint
		{
			return rect.topRight();
		});
	}
	else
	{
		while (!resizeGrips.isEmpty())
		{
			resizeGrips.takeFirst()->deleteLater();
		}
	}
}

void MainWindow::addSizeGrip(const std::function<QPoint(const QRect &)> &position)
{
	auto *sizeGrip = new HiddenSizeGrip(position, this);
	resizeGrips.append(sizeGrip);
}

void MainWindow::minimize()
{
	// HACK: Qt windows that are restored are sometimes still marked minimized
	// so we have to remove that flag if present before trying to minimize.
	//
	// See https://github.com/kraxarn/spotify-qt/issues/103
	setWindowState(windowState() & ~Qt::WindowMinimized);
	setWindowState(windowState() | Qt::WindowMinimized);
}

void MainWindow::refresh()
{
	constexpr int msInSec = 1000;

	if (refreshCount < 0
		|| ++refreshCount >= settings.general.refresh_interval
		|| current.playback.progress_ms + msInSec > current.playback.item.duration)
	{
		spotify->current_playback([this](const lib::spt::playback &playback)
		{
			refreshed(playback);
		});
		refreshCount = 0;
		return;
	}

	// Assume last refresh was 1 sec ago
	if (!current.playback.is_playing)
	{
		return;
	}
	current.playback.progress_ms += msInSec;
	refreshed(current.playback);
}

void MainWindow::refreshed(const lib::spt::playback &playback)
{
	current.playback = playback;

	if (!current.playback.item.is_valid())
	{
		toolBar->setPlaying(false);
		contextView->resetCurrentlyPlaying();
		setWindowTitle("spotify-qt");
		return;
	}

	const auto &currPlaying = current.playback.item;
	if (contextView->getCurrentlyPlaying().id != currPlaying.id
		|| windowTitle() == "spotify-qt")
	{
		if (current.playback.is_playing)
		{
			songs->setPlayingTrackItem(currPlaying.id);
		}

		contextView->setCurrentlyPlaying(currPlaying);
		setAlbumImage(currPlaying.image);
		setWindowTitle(QString::fromStdString(currPlaying.title()));
		contextView->updateContextIcon();

#ifdef USE_DBUS
		if (mediaPlayer != nullptr)
		{
			mediaPlayer->currentSourceChanged(current.playback);
		}
#endif

		if (trayIcon != nullptr && settings.general.tray_album_art)
		{
			HttpUtils::getAlbum(current.playback.item.image, *httpClient,
				cache, [this](const QPixmap &image)
				{
					if (this->trayIcon != nullptr)
					{
						trayIcon->setPixmap(image);
					}
				});
		}
	}

	toolBar->setProgress(current.playback);
	toolBar->setPlaying(current.playback.is_playing);

	if (!settings.general.pulse_volume)
	{
		constexpr int volumeStep = 5;
		toolBar->setVolume(current.playback.volume() / volumeStep);
	}

	toolBar->setRepeat(current.playback.repeat);
	toolBar->setShuffle(current.playback.shuffle);
}

auto MainWindow::createCentralWidget() -> QWidget *
{
	// All widgets in container
	songs = new TracksList(*spotify, settings, cache, this);
	sidePanel = new View::SidePanel::SidePanel(*spotify, settings, cache,
		*httpClient, this);

	libraryList = new LibraryList(*spotify, this);
	playlistList = new PlaylistList(*spotify, settings, cache, this);
	contextView = new View::Context::Context(*spotify, current, cache, this);

	// Left side panel
	addDockWidget(Qt::LeftDockWidgetArea,
		WidgetUtils::createDockWidget(libraryList, "Library",
			Widget::DockTitle::margins(), this));
	addDockWidget(Qt::LeftDockWidgetArea,
		WidgetUtils::createDockWidget(playlistList, "Playlists",
			Widget::DockTitle::margins(), this));
	addDockWidget(Qt::LeftDockWidgetArea, contextView);

	// Right side panel
	addDockWidget(Qt::RightDockWidgetArea, sidePanel);

	return songs;
}

auto MainWindow::startClient() -> bool
{
	stopClient();

	sptClient = new spt::ClientHandler(settings, paths, this);
	auto status = sptClient->start();
	if (!status.isEmpty())
	{
		QMessageBox::warning(this, "Client error",
			QString("Failed to start Spotify client: %1").arg(status));
		return false;
	}

	return true;
}

void MainWindow::stopClient()
{
	delete sptClient;
	sptClient = nullptr;
}

void MainWindow::openAudioFeaturesWidget(const lib::spt::track &track)
{
	dynamic_cast<View::SidePanel::SidePanel *>(sidePanel)->openAudioFeatures(track);
}

void MainWindow::openLyrics(const lib::spt::track &track)
{
	dynamic_cast<View::SidePanel::SidePanel *>(sidePanel)->openLyrics(track);
}

auto MainWindow::loadAlbum(const std::string &albumId, const std::string &trackId) -> bool
{
	spotify->album(albumId, [this, trackId](const lib::spt::album &album)
	{
		this->songs->load(album, trackId);
	});
	return true;
}

auto MainWindow::loadTracksFromCache(const std::string &id) -> std::vector<lib::spt::track>
{
	return cache.get_tracks(id);
}

void MainWindow::saveTracksToCache(const std::string &id,
	const std::vector<lib::spt::track> &tracks)
{
	cache.set_tracks(id, tracks);
}

void MainWindow::setStatus(const QString &message, bool important)
{
	if (message.isNull() || message.isEmpty())
	{
		return;
	}

	if (trayIcon != nullptr && settings.general.tray_notifications)
	{
		if (important)
		{
			trayIcon->message(message);
		}
	}
	else
	{
		const int messageTimeout = 5000;
		statusBar()->showMessage(message, messageTimeout);
	}
}

void MainWindow::status(const std::string &message, bool important)
{
	setStatus(QString::fromStdString(message), important);
}

void MainWindow::setAlbumImage(const std::string &url)
{
	HttpUtils::getAlbum(url, *httpClient, cache, [this](const QPixmap &image)
	{
		if (this->contextView != nullptr)
		{
			contextView->setAlbum(image);
		}
	});
}

void MainWindow::openArtist(const std::string &artistId)
{
	dynamic_cast<View::SidePanel::SidePanel *>(sidePanel)->openArtist(artistId);
}

auto MainWindow::currentTracks() -> std::vector<std::string>
{
	std::vector<std::string> tracks;
	tracks.reserve(songs->topLevelItemCount());

	for (int i = 0; i < songs->topLevelItemCount(); i++)
	{
		auto track = songs->topLevelItem(i)->data(0, RoleTrack)
			.value<lib::spt::track>();
		if (!track.is_valid())
		{
			continue;
		}
		tracks.push_back(lib::spt::api::to_uri("track", track.id));
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
	{
		trayIcon = new TrayIcon(spotify, settings, this);
	}
}

void MainWindow::setFixedWidthTime(bool value)
{
	toolBar->setPositionFont(value
		? QFont("monospace")
		: QFont());
}

void MainWindow::toggleTrackNumbers(bool enabled)
{
	for (int i = 0; i < songs->topLevelItemCount(); i++)
	{
		auto *item = songs->topLevelItem(i);
		item->setText(0, enabled
			? QString("%1").arg(item->data(0, RoleIndex).toInt() + 1, 3)
			: QString());
	}
}

//region Getters

auto MainWindow::currentPlayback() const -> lib::spt::playback
{
	return current.playback;
}

auto MainWindow::getCurrentUser() -> lib::spt::user
{
	return currentUser;
}

void MainWindow::setSearchChecked(bool checked)
{
	toolBar->setSearchChecked(checked);
}

auto MainWindow::getSongsTree() -> TracksList *
{
	return songs;
}

auto MainWindow::getSptContext() const -> std::string
{
	return current.context.toStdString();
}

void MainWindow::setSptContext(const std::string &uri)
{
	current.context = QString::fromStdString(uri);
}

void MainWindow::setSptContext(const lib::spt::playlist &playlist)
{
	setSptContext(lib::spt::api::to_uri("playlist", playlist.id));
}

void MainWindow::setSptContext(const lib::spt::album &album)
{
	setSptContext(lib::spt::api::to_uri("album", album.id));
	playlistList->setCurrentRow(-1);
}

void MainWindow::setNoSptContext()
{
	setSptContext(std::string());
}

auto MainWindow::getCurrentPlayback() -> lib::spt::playback &
{
	return current.playback;
}

auto MainWindow::getCurrent() -> const spt::Current &
{
	return current;
}

auto MainWindow::getClientHandler() -> const spt::ClientHandler *
{
	return sptClient;
}

#ifdef USE_DBUS
auto MainWindow::getMediaPlayer() -> mp::Service *
{
	return mediaPlayer;
}
#endif

auto MainWindow::isValid() const -> bool
{
	return stateValid;
}

//endregion

void MainWindow::setSearchVisible(bool visible)
{
	auto *panel = dynamic_cast<View::SidePanel::SidePanel *>(sidePanel);
	if (visible)
	{
		panel->openSearch();
	}
	else
	{
		panel->closeSearch();
	}
}

void MainWindow::addSidePanelTab(QWidget *widget, const QString &title)
{
	dynamic_cast<View::SidePanel::SidePanel *>(sidePanel)->addTab(widget, "folder-txt", title);
}

void MainWindow::refreshPlaylists()
{
	playlistList->refresh();
}

void MainWindow::setCurrentLibraryItem(QTreeWidgetItem *item)
{
	libraryList->setCurrentItem(item);
}

auto MainWindow::getPlaylist(int index) -> lib::spt::playlist
{
	return playlistList->at(index);
}

void MainWindow::setCurrentPlaylistItem(int index)
{
	playlistList->setCurrentRow(index);
}

auto MainWindow::allArtists() -> std::unordered_set<std::string>
{
	return playlistList->allArtists();
}

auto MainWindow::getCurrentPlaylistItem() -> QListWidgetItem *
{
	return playlistList->currentItem();
}

auto MainWindow::getPlaylistItemCount() -> int
{
	return playlistList->count();
}

auto MainWindow::getPlaylistItem(int index) -> QListWidgetItem *
{
	return playlistList->item(index);
}

void MainWindow::orderPlaylists(lib::playlist_order order)
{
	playlistList->order(order);
}

auto MainWindow::find(QWidget *from) -> MainWindow *
{
	return WidgetUtils::find<MainWindow>(from);
}

auto MainWindow::defaultSize() -> QSize
{
	constexpr int width = 1280;
	constexpr int height = 720;

	return {
		width,
		height
	};
}

void MainWindow::resetLibraryPlaylist() const
{
	libraryList->setCurrentItem(nullptr);
	playlistList->setCurrentRow(-1);
}
