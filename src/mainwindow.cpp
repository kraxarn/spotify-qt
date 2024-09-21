#include "mainwindow.hpp"
#include "lib/time.hpp"
#include "lib/system.hpp"
#include "util/widget.hpp"
#include "util/font.hpp"
#include "menu/mainmenubar.hpp"
#include "util/appconfig.hpp"

#ifdef _WIN32
#include "windows.h"
#endif

MainWindow::MainWindow(lib::settings &settings, lib::paths &paths,
	lib::qt::http_client &httpClient, lib::spt::api &spotify)
	: spotify(spotify),
	settings(settings),
	paths(paths),
	cache(paths),
	httpClient(httpClient)
{
	lib::crash_handler::set_cache(cache);

	// winId is required for moving the window under Wayland
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
	if (QGuiApplication::platformName() == "wayland" || settings.general.native_window)
	{
		winId();
	}
#endif

	Style::apply(this, settings);

	// Update player status
	auto *timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout, this, &MainWindow::refresh);
	refresh();
	constexpr int tickMs = 1000;
	timer->start(tickMs);

	// Start client if set
	initClient();

	// Start media controller if specified
	initMediaController();

	// Register media hotkeys
	initMediaHotkeys();

	// Create tray icon if specified
	if (settings.general.tray_icon)
	{
		trayIcon = new TrayIcon(spotify, settings, cache, httpClient, this);
	}

	// If new version has been detected, show what's new dialog
	initWhatsNew();
	checkForUpdates();

	// Get current user
	spotify.me([this](const lib::spt::user &user)
	{
		this->currentUser = user;
	});

	initDevice();

	setWindowTitle(APP_NAME);
	setWindowIcon(Icon::get(QString("logo:%1").arg(APP_ICON)));
	resize(defaultSize());
	setCentralWidget(createCentralWidget());
	toolBar = new MainToolBar(spotify, settings, httpClient, cache, this);
	const auto toolbarArea = MainToolBar::toToolBarArea(settings.qt().toolbar_position);
	addToolBar(toolbarArea, toolBar);
	setContextMenuPolicy(Qt::NoContextMenu);

	setBorderless(!settings.qt().system_title_bar);

	if (AppConfig::useNativeMenuBar())
	{
		new MainMenuBar(spotify, settings, httpClient, cache, this);
	}

	QCoreApplication::instance()->installEventFilter(this);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (settings.general.close_to_tray && trayIcon != nullptr)
	{
		event->ignore();
		this->setVisible(false);
	}
	else
	{
		trayIcon->deleteLater();
		event->accept();
		mainContent = nullptr;
	}
}

auto MainWindow::eventFilter(QObject *obj, QEvent *event) -> bool
{
	if (event->type() != QEvent::MouseButtonPress)
	{
		return QMainWindow::eventFilter(obj, event);
	}

	auto *mouseEvent = dynamic_cast<QMouseEvent *>(event);

	if (mouseEvent->button() == Qt::BackButton)
	{
		history()->back();
		return true;
	}

	if (mouseEvent->button() == Qt::ForwardButton)
	{
		history()->forward();
		return true;
	}

	return QMainWindow::eventFilter(obj, event);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#else
bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
#endif
{
#ifdef _WIN32
	MSG* msg = reinterpret_cast<MSG*>(message);
	if (msg->message == WM_HOTKEY) {
		UINT key = msg->wParam;
		switch (key)
		{
			case 0: //Next Track
			{
				spotify.next([this](const std::string &status)
				{
					if (!status.empty())
					{
						StatusMessage::error(QString("Failed to go to next track: %1")
							.arg(QString::fromStdString(status)));
						return;
					}
					refresh();
				});
				break;
			}

			case 1: //Previous Track
			{
				spotify.previous([this](const std::string &status)
				{
					if (!status.empty())
					{
						StatusMessage::error(QString("Failed to go to previous track: %1")
							.arg(QString::fromStdString(status)));
						return;
					}
					refresh();
				});
				break;
			}

			case 2: //Stop Media
			{
				auto current = playback();
				current.is_playing = false;
				refreshed(current);
				auto callback = [this](const std::string &status)
				{
					if (status.empty())
					{
						return;
					}

					StatusMessage::error(QString("Failed to stop playback: %1")
						.arg(QString::fromStdString(status)));
				};
				spotify.pause(callback);
				break;
			}

			case 3: //Play/Pause Media
			{
				auto current = playback();
				current.is_playing = !current.is_playing;
				refreshed(current);
				auto callback = [this](const std::string &status)
				{
					if (status.empty())
					{
						return;
					}

					StatusMessage::error(QString("Failed to play/pause playback: %1")
						.arg(QString::fromStdString(status)));
				};
				if (current.is_playing)
				{
					spotify.resume(callback);
				}
				else
				{
					spotify.pause(callback);
				}
				break;
			}
		}
	}
#endif
	return QMainWindow::nativeEvent(eventType, message, result);
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
		spotify.devices([this](const std::vector<lib::spt::device> &devices)
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

void MainWindow::initMediaHotkeys()
{
#ifdef _WIN32
	if (settings.general.media_hotkeys)
	{
		registerMediaHotkeys(true);
	}
#endif
}

#ifdef _WIN32
void MainWindow::registerMediaHotkeys(bool enabled)
{
	HWND hwnd = HWND(winId());
	if (enabled)
	{
		RegisterHotKey(hwnd, 0, MOD_NOREPEAT, 0xB0); // Next Track
		RegisterHotKey(hwnd, 1, MOD_NOREPEAT, 0xB1); // Previous Track
		RegisterHotKey(hwnd, 2, MOD_NOREPEAT, 0xB2); // Stop Media
		RegisterHotKey(hwnd, 3, MOD_NOREPEAT, 0xB3); // Play/Pause Media
	}
	else
	{
		UnregisterHotKey(hwnd, 0);
		UnregisterHotKey(hwnd, 1);
		UnregisterHotKey(hwnd, 2);
		UnregisterHotKey(hwnd, 3);
	}
}
#endif

void MainWindow::initWhatsNew()
{
	if (settings.general.show_changelog
		&& !settings.general.last_version.empty()
		&& settings.general.last_version != APP_VERSION)
	{
		auto *dialog = new Dialog::WhatsNew(settings, httpClient, this);
		dialog->open();
	}

	settings.general.last_version = APP_VERSION;
	settings.save();
}

void MainWindow::initDevice()
{
	spotify.devices([this](const std::vector<lib::spt::device> &devices)
	{
		// Don't select a new device if one is currently active
		for (const auto &device: devices)
		{
			if (device.is_active)
			{
				return;
			}
		}

		if (devices.size() == 1
			&& lib::strings::starts_with(devices.front().name, APP_NAME))
		{
			spotify.set_device(devices.front(), {});
		}
		else
		{
			for (const auto &device: devices)
			{
				if (device.id == this->settings.general.last_device)
				{
					spotify.set_device(device, {});
					break;
				}
			}
		}
	});
}

void MainWindow::checkForUpdates()
{
	if (!settings.general.check_for_updates)
	{
		return;
	}

	const auto *const url = "https://api.github.com/repos/kraxarn/spotify-qt/releases/latest";
	httpClient.get(url, lib::headers(), [this](const std::string &data)
	{
		if (data.empty())
		{
			return;
		}

		auto json = nlohmann::json::parse(data);
		if (!json.contains("tag_name"))
		{
			return;
		}

		const auto &latest = json.at("tag_name").get<std::string>();
		if (latest.empty() || latest == APP_VERSION)
		{
			return;
		}

		const auto text = QString("%1 %2 is now available")
			.arg(APP_NAME, QString::fromStdString(latest));

		StatusMessage::info(text, QStringLiteral("Open release"), [this]()
		{
			const QString url("https://github.com/kraxarn/spotify-qt/releases/latest");
			Url::open(url, LinkType::Web, this);
		});
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
	if (refreshCount < 0
		|| ++refreshCount >= settings.general.refresh_interval
		|| current.playback.progress_ms + lib::time::ms_in_sec > current.playback.item.duration)
	{
		spotify.current_playback([this](const lib::result<lib::spt::playback> &result)
		{
			if (result.success())
			{
				refreshed(result.value());
				refreshCount = 0;
				return;
			}

			if (!result.message().empty())
			{
				lib::log::error("Refresh failed: {}", result.message());
			}

			if (current.playback.is_playing)
			{
				current.playback.progress_ms += lib::time::ms_in_sec;
				refreshed(current.playback);
			}
		});
		return;
	}

	// Assume last refresh was 1 sec ago
	if (!current.playback.is_playing)
	{
		return;
	}
	current.playback.progress_ms += lib::time::ms_in_sec;
	refreshed(current.playback);
}

void MainWindow::refreshed(const lib::spt::playback &playback)
{
	const auto previous = current.playback;
	const auto trackChange = previous.item.id != playback.item.id;
	current.playback = playback;

	emit playbackRefreshed(playback, previous);

	if (!current.playback.item.is_valid())
	{
		setWindowTitle(APP_NAME);
		return;
	}

	if (current.playback.device.is_valid())
	{
		settings.general.last_device = current.playback.device.id;
	}

	if (trackChange || windowTitle() == APP_NAME)
	{
		const auto &qtSettings = settings.qt();
		const auto title = lib::format::title(playback.item, qtSettings.track_title);
		setWindowTitle(QString::fromStdString(title));
	}
}

auto MainWindow::createCentralWidget() -> QWidget *
{
	// All widgets in container
	mainContent = new MainContent(spotify, settings, cache, httpClient, this);
	sidePanel = new SidePanel::View(spotify, settings, cache, httpClient, this);
	sidePanel->setVisible(false);

	libraryList = new List::Library(spotify, cache, httpClient, settings, this);
	playlistList = new List::Playlist(spotify, settings, cache, httpClient, this);
	contextView = new Context::View(spotify, settings, cache, httpClient, this);

	auto *libraryDock = Widget::createDockWidget(libraryList,
		QStringLiteral("Library"), DockTitle::margins(), this);

	auto *playlistListDock = Widget::createDockWidget(playlistList,
		QStringLiteral("Playlists"), DockTitle::margins(), this);

	addDockWidget(Qt::LeftDockWidgetArea, libraryDock);
	addDockWidget(Qt::LeftDockWidgetArea, playlistListDock);

	if (settings.qt().library_layout == lib::library_layout::tabbed)
	{
		tabifyDockWidget(libraryDock, playlistListDock);
	}

	addDockWidget(Qt::LeftDockWidgetArea, contextView);
	addDockWidget(Qt::RightDockWidgetArea, sidePanel);

	return mainContent;
}

auto MainWindow::getDockedWidgets() -> std::string
{
	nlohmann::json json;
	QMap<Qt::DockWidgetArea, int> indexes;

	auto widgets = findChildren<QDockWidget *>(QString(), Qt::FindDirectChildrenOnly);
	for (auto *widget: widgets)
	{
		const auto key = (widget->objectName().isEmpty()
			? QString(widget->metaObject()->className())
			: widget->objectName())
			.toStdString();

		if (json.contains(key))
		{
			lib::log::warn("Docked widget with name \"{}\" already exists, ignoring");
			continue;
		}

		const auto area = dockWidgetArea(widget);
		int index;

		if (indexes.contains(area))
		{
			index = ++indexes[area];
		}
		else
		{
			index = indexes[area] = 0;
		}

		std::vector<std::string> tabNames;
		for (const auto *tabbedWidget: tabifiedDockWidgets(widget))
		{
			tabNames.push_back((tabbedWidget->objectName().isEmpty()
				? QString(tabbedWidget->metaObject()->className())
				: tabbedWidget->objectName())
				.toStdString());
		}

		nlohmann::json obj{
			{"y",     qMax(widget->pos().y(), -1)},
			{"index", index},
			{"area",  area},
			{"tabs",  tabNames},
		};

		const auto y = widget->pos().y();
		if (y >= 0)
		{
			obj["y"] = (nlohmann::json::value_type) y;
		}
		else
		{
			obj["y"] = nullptr;
		}

		json[key] = obj;
	}

	return json.dump();
}

auto MainWindow::startClient() -> const SpotifyClient::Runner *
{
	stopClient();

	spotifyRunner = new SpotifyClient::Runner(settings, paths, this);
	SpotifyClient::Runner::connect(spotifyRunner, &SpotifyClient::Runner::statusChanged,
		this, &MainWindow::onSpotifyStatusChanged);

	spotifyRunner->start();
	return spotifyRunner;
}

void MainWindow::stopClient()
{
	if (spotifyRunner != nullptr){
		spotifyRunner->deleteLater();
		spotifyRunner = nullptr;
	}
}

void MainWindow::openLyrics(const lib::spt::track &track)
{
	dynamic_cast<SidePanel::View *>(sidePanel)->openLyrics(track);
}

void MainWindow::loadAlbum(const std::string &albumId, const std::string &trackId)
{
	auto *tracksList = mainContent->getTracksList();

	const auto album = cache.get_album(albumId);
	if (album.is_valid())
	{
		tracksList->load(album, trackId);
		return;
	}

	spotify.album(albumId, [tracksList, trackId](const lib::spt::album &album)
	{
		tracksList->load(album, trackId);
	});
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

void MainWindow::openArtist(const std::string &artistId)
{
	dynamic_cast<SidePanel::View *>(sidePanel)->openArtist(artistId);
}

auto MainWindow::currentTracks() -> std::vector<std::string>
{
	const auto *tracksList = mainContent->getTracksList();
	const auto tracksCount = tracksList->topLevelItemCount();

	std::vector<std::string> tracks;
	tracks.reserve(tracksCount);

	for (int i = 0; i < tracksCount; i++)
	{
		auto track = tracksList->topLevelItem(i)->data(0, static_cast<int>(DataRole::Track))
			.value<lib::spt::track>();
		if (!track.is_valid())
		{
			continue;
		}
		tracks.push_back(lib::spt::id_to_uri("track", track.id));
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
		trayIcon = new TrayIcon(spotify, settings, cache, httpClient, this);
	}
}

auto MainWindow::getTrayIcon() -> TrayIcon *
{
	return trayIcon;
}

void MainWindow::toggleTrackNumbers(bool enabled)
{
	const auto *tracksList = mainContent->getTracksList();

	for (int i = 0; i < tracksList->topLevelItemCount(); i++)
	{
		auto *item = tracksList->topLevelItem(i);
		item->setText(0, enabled
			? QString("%1").arg(item->data(0, static_cast<int>(DataRole::Index))
				.toInt() + 1, 3)
			: QString());
	}
}

void MainWindow::toggleExpandableAlbum(lib::album_size albumSize)
{
	contextView->setAlbumSize(albumSize);
}

//region Getters

auto MainWindow::playback() const -> const lib::spt::playback &
{
	return current.playback;
}

auto MainWindow::getCurrentUser() const -> const lib::spt::user &
{
	return currentUser;
}

void MainWindow::setSearchChecked(bool checked)
{
	toolBar->setSearchChecked(checked);
}

auto MainWindow::getSongsTree() -> List::Tracks *
{
	return mainContent->getTracksList();
}

auto MainWindow::history() -> HistoryButton *
{
	return toolBar->findChild<HistoryButton *>({}, Qt::FindDirectChildrenOnly);
}

auto MainWindow::getCurrent() -> const spt::Current &
{
	return current;
}

auto MainWindow::getSpotifyRunner() -> const SpotifyClient::Runner *
{
	return spotifyRunner;
}

#ifdef USE_DBUS
auto MainWindow::getMediaPlayer() -> mp::Service *
{
	return mediaPlayer;
}
#endif

//endregion

void MainWindow::setSearchVisible(bool visible)
{
	auto *panel = dynamic_cast<SidePanel::View *>(sidePanel);
	if (visible)
	{
		panel->openSearch();
	}
	else
	{
		panel->closeSearch();
	}
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

auto MainWindow::find(QObject *from) -> MainWindow *
{
	auto *active = qobject_cast<MainWindow *>(QApplication::activeWindow());
	if (active != nullptr)
	{
		return active;
	}

	auto *fromWidget = qobject_cast<QWidget *>(from);
	return Parent::findWidget<MainWindow>(fromWidget);
}

auto MainWindow::defaultSize() -> QSize
{
	constexpr int width = 1280;
	constexpr int height = 720;

	return {
		width,
		height,
	};
}

void MainWindow::resetLibraryPlaylist() const
{
	libraryList->setCurrentItem(nullptr);
	playlistList->setCurrentRow(-1);
}

void MainWindow::onSpotifyStatusChanged(const QString &status)
{
	if (status.isEmpty() || mainContent == nullptr)
	{
		return;
	}

	const auto message = QString("Spotify client error: %1")
		.arg(status);

	if ((windowState() & Qt::WindowActive) > 0)
	{
		StatusMessage::warn(message);
	}
	else
	{
		QMessageBox::warning(this, QStringLiteral("Client error"), message);
	}
}
