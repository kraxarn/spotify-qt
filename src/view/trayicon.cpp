#include "trayicon.hpp"

#include "mainwindow.hpp"
#include "util/http.hpp"

TrayIcon::TrayIcon(lib::spt::api &spotify, lib::settings &settings, lib::cache &cache,
	const HttpClient &httpClient, QWidget *parent)
	: QSystemTrayIcon(parent),
	spotify(spotify),
	settings(settings),
	cache(cache),
	httpClient(httpClient)
{
	callback = [this](const std::string &result)
	{
		message(QString::fromStdString(result));
	};

	contextMenu = new QMenu(parent);
	currentTrack = contextMenu->addAction("-");
	currentTrack->setEnabled(false);
	contextMenu->addSeparator();

	const auto previousIcon = Icon::get("media-skip-backward");
	previous = contextMenu->addAction(previousIcon, "Previous");
	QAction::connect(previous, &QAction::triggered,
		this, &TrayIcon::onPrevious);

	const auto playPauseIcon = Icon::get("media-playback-start");
	playPause = contextMenu->addAction(playPauseIcon, "Play");
	QAction::connect(playPause, &QAction::triggered,
		this, &TrayIcon::onPlayPause);

	next = contextMenu->addAction(Icon::get("media-skip-forward"), "Next");
	QAction::connect(next, &QAction::triggered,
		this, &TrayIcon::onNext);

	contextMenu->addSeparator();

	showApp = contextMenu->addAction(Icon::get("window"), QStringLiteral("Show"));
	showApp->setVisible(settings.general.close_to_tray);
	QAction::connect(showApp, &QAction::triggered, this, &TrayIcon::onShowWindow);

	auto *quit = contextMenu->addAction(Icon::get("application-exit"), "Quit");
	QAction::connect(quit, &QAction::triggered, &QCoreApplication::quit);

	setDefaultPixmap();
	setContextMenu(contextMenu);
	show();

	QSystemTrayIcon::connect(this, &QSystemTrayIcon::activated,
		this, &TrayIcon::onActivated);

	QMenu::connect(contextMenu, &QMenu::aboutToShow,
		this, &TrayIcon::onMenuAboutToShow);

	auto *mainWindow = MainWindow::find(parent);
	MainWindow::connect(mainWindow, &MainWindow::playbackRefreshed,
		this, &TrayIcon::onPlaybackRefreshed);
}

TrayIcon::~TrayIcon()
{
	contextMenu->deleteLater();
}

void TrayIcon::message(const QString &message)
{
	if (message.isNull() || message.isEmpty())
	{
		return;
	}

	showMessage(APP_NAME, message);
}

void TrayIcon::message(const lib::spt::track &track, const QPixmap &pixmap)
{
	if (!track.is_valid())
	{
		return;
	}

	const auto title = QString::fromStdString(track.name);
	const auto artists = lib::spt::entity::combine_names(track.artists);
	const auto message = QString::fromStdString(artists);

	QIcon icon(pixmap);
	showMessage(title, message, icon, messageTrackTimeout);
}

auto TrayIcon::playback() -> lib::spt::playback
{
	auto *mainWindow = qobject_cast<MainWindow*>(this->parent());
	if (mainWindow == nullptr)
	{
		return {};
	}
	return mainWindow->playback();
}

void TrayIcon::setPixmap(const QPixmap &pixmap)
{
	const auto albumShape = settings.qt().album_shape;
	setIcon(Image::mask(pixmap, albumShape));
}

void TrayIcon::setDefaultPixmap()
{
	QIcon icon;

	if (settings.general.tray_system_icon)
	{
		// Use QIcon directly to avoid automatic fallback
		icon = QIcon::fromTheme(QStringLiteral("%1-symbolic")
			.arg(QStringLiteral(APP_NAME)));
	}

	if (icon.isNull())
	{
		icon = Icon::get(QStringLiteral("logo:%1-symbolic-%2")
			.arg(QStringLiteral(APP_ICON))
			.arg(settings.general.tray_light_icon
				? QStringLiteral("light")
				: QStringLiteral("dark")));
	}

	setIcon(icon);
}

void TrayIcon::showWindow()
{
	auto *parentWidget = qobject_cast<QWidget*>(parent());
	if (parentWidget != nullptr)
	{
		parentWidget->setVisible(!parentWidget->isVisible());
	}
}

void TrayIcon::onPrevious(bool /*checked*/)
{
	spotify.previous(callback);
}

void TrayIcon::onPlayPause(bool /*checked*/)
{
	if (playback().is_playing)
	{
		spotify.pause(callback);
	}
	else
	{
		spotify.resume(callback);
	}
}

void TrayIcon::onNext(bool /*checked*/)
{
	spotify.next(callback);
}

void TrayIcon::onActivated(ActivationReason reason)
{
	if (reason == ActivationReason::Trigger)
	{
		showWindow();
	}
}

void TrayIcon::onShowWindow(bool /*checked*/)
{
	showWindow();
}

void TrayIcon::onMenuAboutToShow()
{
	auto current = playback();
	auto isPlaying = current.is_playing;

	const auto &qtSettings = settings.qt();
	const QString title = Format::title(current.item, QString::fromStdString(qtSettings.track_title));
	currentTrack->setText(title);

	playPause->setIcon(Icon::get(isPlaying
		? "media-playback-pause"
		: "media-playback-start"));

	playPause->setText(isPlaying
		? "Pause"
		: "Play");

#ifdef __APPLE__
	if (showApp != nullptr)
	{
		auto *parentWidget = qobject_cast<QWidget*>(parent());
		if (parentWidget != nullptr)
		{
			showApp->setText(parentWidget->isVisible()
				? QStringLiteral("Hide")
				: QStringLiteral("Show"));
		}
	}
#endif
}

void TrayIcon::onPlaybackRefreshed(const lib::spt::playback &currentPlayback,
	const lib::spt::playback &previousPlayback)
{
	if (!settings.general.tray_album_art && !settings.general.notify_track_change)
	{
		return;
	}

	if (!currentPlayback.is_valid())
	{
		setDefaultPixmap();
		return;
	}

	Http::getAlbumImage(currentPlayback.item.image_small(), httpClient, cache, false,
		[this, currentPlayback, previousPlayback](const QPixmap &image)
		{
			const auto trackChange = currentPlayback.is_playing
				&& previousPlayback.is_playing
				&& currentPlayback.item.id != previousPlayback.item.id;

			if (settings.general.tray_album_art)
			{
				setPixmap(image);
			}

			if (settings.general.notify_track_change && trackChange)
			{
				message(currentPlayback.item, image);
			}
		});
}
