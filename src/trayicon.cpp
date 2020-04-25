#include "trayicon.hpp"

TrayIcon::TrayIcon(spt::Spotify *spotify, QObject *parent) : QSystemTrayIcon(parent), spotify(spotify)
{
	contextMenu = new QMenu();
	auto previous = contextMenu->addAction(Icon::get("media-skip-backward"), "Previous");
	QAction::connect(previous, &QAction::triggered, [spotify]() {
		spotify->previous();
	});
	auto next = contextMenu->addAction(Icon::get("media-skip-forward"),  "Next");
	QAction::connect(next, &QAction::triggered, [spotify]() {
		spotify->next();
	});
	playPause = contextMenu->addAction(Icon::get("media-playback-start"), "Play");
	QAction::connect(playPause, &QAction::triggered, [this](bool checked) {
		if (isPlaying())
			this->spotify->pause();
		else
			this->spotify->resume();
	});
	contextMenu->addSeparator();
	contextMenu->addAction(Icon::get("application-exit"), "Quit");

	setIcon(QIcon::fromTheme("spotify-qt", Icon::get("logo:spotify-qt")));
	setContextMenu(contextMenu);
	show();

	QSystemTrayIcon::connect(this, &QSystemTrayIcon::activated, [this](ActivationReason reason) {
		if (reason != ActivationReason::Trigger)
			return;
		auto window = dynamic_cast<QWidget*>(this->parent());
		if (window != nullptr)
			window->setVisible(!window->isVisible());
	});

	QMenu::connect(contextMenu, &QMenu::aboutToShow, [this]() {
		auto playing = isPlaying();
		playPause->setIcon(Icon::get(playing ? "media-playback-pause" : "media-playback-start"));
		playPause->setText(playing ? "Pause" : "Play");
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

bool TrayIcon::isPlaying()
{
	auto mainWindow = dynamic_cast<MainWindow*>(this->parent());
	if (mainWindow == nullptr)
		return false;
	return mainWindow->currentPlayback().isPlaying;
}
