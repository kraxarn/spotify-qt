#include "trayicon.hpp"

TrayIcon::TrayIcon(spt::Spotify *spotify, const lib::settings &settings, QObject *parent)
	: spotify(spotify), settings(settings), QSystemTrayIcon(parent)
{
	contextMenu = new QMenu();
	currentTrack = contextMenu->addAction("-");
	currentTrack->setEnabled(false);
	contextMenu->addSeparator();

	auto previous = contextMenu->addAction(Icon::get("media-skip-backward"), "Previous");
	QAction::connect(previous, &QAction::triggered, [spotify]()
	{
		spotify->previous();
	});

	playPause = contextMenu->addAction(Icon::get("media-playback-start"), "Play");
	QAction::connect(playPause, &QAction::triggered, [this](bool checked)
	{
		if (playback().isPlaying)
			this->spotify->pause();
		else
			this->spotify->resume();
	});

	auto next = contextMenu->addAction(Icon::get("media-skip-forward"), "Next");
	QAction::connect(next, &QAction::triggered, [spotify]()
	{
		spotify->next();
	});

	contextMenu->addSeparator();
	auto quit = contextMenu->addAction(Icon::get("application-exit"), "Quit");
	QAction::connect(quit, &QAction::triggered, QCoreApplication::quit);

	setIcon(Icon::get(QString("logo:spotify-qt-symbolic-%1")
		.arg(settings.general.tray_light_icon ? "light" : "dark")).pixmap(64, 64));
	setContextMenu(contextMenu);
	show();

	QSystemTrayIcon::connect(this, &QSystemTrayIcon::activated, [this](ActivationReason reason)
	{
		if (reason != ActivationReason::Trigger)
			return;
		auto window = dynamic_cast<QWidget *>(this->parent());
		if (window != nullptr)
			window->setVisible(!window->isVisible());
	});

	QMenu::connect(contextMenu, &QMenu::aboutToShow, [this]()
	{
		auto current = playback();
		currentTrack->setText(QString("%1 - %2")
			.arg(current.item.artist)
			.arg(current.item.name));
		auto isPlaying = current.isPlaying;
		playPause->setIcon(Icon::get(isPlaying ? "media-playback-pause" : "media-playback-start"));
		playPause->setText(isPlaying ? "Pause" : "Play");
	});
}

TrayIcon::~TrayIcon()
{
	delete contextMenu;
}

void TrayIcon::message(const QString &message)
{
	showMessage("spotify-qt", message);
}

spt::Playback TrayIcon::playback()
{
	auto mainWindow = dynamic_cast<MainWindow *>(this->parent());
	if (mainWindow == nullptr)
		return spt::Playback();
	return mainWindow->currentPlayback();
}

void TrayIcon::setPixmap(const QPixmap &pixmap)
{
	setIcon(Utils::mask(pixmap));
}
