#include "qttrayicon.hpp"
#include "mainwindow.hpp"

QtTrayIcon::QtTrayIcon(lib::spt::api &spotify, const lib::settings &settings, QWidget *parent)
	: QSystemTrayIcon(parent),
	spotify(spotify),
	settings(settings)
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
		this, &QtTrayIcon::onPrevious);

	const auto playPauseIcon = Icon::get("media-playback-start");
	playPause = contextMenu->addAction(playPauseIcon, "Play");
	QAction::connect(playPause, &QAction::triggered,
		this, &QtTrayIcon::onPlayPause);

	next = contextMenu->addAction(Icon::get("media-skip-forward"), "Next");
	QAction::connect(next, &QAction::triggered,
		this, &QtTrayIcon::onNext);

	contextMenu->addSeparator();
	auto *quit = contextMenu->addAction(Icon::get("application-exit"), "Quit");
	QAction::connect(quit, &QAction::triggered, QCoreApplication::quit);

	setDefaultPixmap();
	setContextMenu(contextMenu);
	show();

	QSystemTrayIcon::connect(this, &QSystemTrayIcon::activated,
		this, &QtTrayIcon::onActivated);

	QMenu::connect(contextMenu, &QMenu::aboutToShow,
		this, &QtTrayIcon::onMenuAboutToShow);
}

QtTrayIcon::~QtTrayIcon()
{
	contextMenu->deleteLater();
}

void QtTrayIcon::message(const QString &message)
{
	if (message.isNull() || message.isEmpty())
	{
		return;
	}

	showMessage("spotify-qt", message);
}

void QtTrayIcon::message(const QString &message, const QIcon &icon)
{
	if (message.isNull() || message.isEmpty())
	{
		return;
	}

	showMessage("spotify-qt", message, icon, messageIconTimeout);
}

auto QtTrayIcon::playback() -> lib::spt::playback
{
	auto *mainWindow = qobject_cast<MainWindow *>(this->parent());
	if (mainWindow == nullptr)
	{
		return {};
	}
	return mainWindow->currentPlayback();
}

void QtTrayIcon::setPixmap(const QPixmap &pixmap)
{
	setIcon(ImageUtils::mask(pixmap));
}

void QtTrayIcon::setDefaultPixmap()
{
	constexpr int iconSize = 64;

	setIcon(Icon::get(QString("logo:spotify-qt-symbolic-%1")
		.arg(settings.general.tray_light_icon ? "light" : "dark"))
		.pixmap(iconSize, iconSize));
}

void QtTrayIcon::onPrevious(bool /*checked*/)
{
	spotify.previous(callback);
}

void QtTrayIcon::onPlayPause(bool /*checked*/)
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

void QtTrayIcon::onNext(bool /*checked*/)
{
	spotify.next(callback);
}

void QtTrayIcon::onActivated(ActivationReason reason)
{
	if (reason != ActivationReason::Trigger)
	{
		return;
	}

	auto *parentWidget = qobject_cast<QWidget *>(parent());
	if (parentWidget != nullptr)
	{
		parentWidget->setVisible(!parentWidget->isVisible());
	}
}

void QtTrayIcon::onMenuAboutToShow()
{
	auto current = playback();
	auto isPlaying = current.is_playing;

	currentTrack->setText(QString::fromStdString(current.item.title()));

	playPause->setIcon(Icon::get(isPlaying
		? "media-playback-pause"
		: "media-playback-start"));

	playPause->setText(isPlaying
		? "Pause"
		: "Play");
}
