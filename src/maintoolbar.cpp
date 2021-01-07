#include "maintoolbar.hpp"

MainToolBar::MainToolBar(spt::Spotify &spotify, lib::settings &settings, QWidget *parent)
	: parent(parent),
	spotify(spotify),
	settings(settings),
	QToolBar("Media controls", parent)
{
	auto mainWindow = dynamic_cast<MainWindow *>(parent);
	if (mainWindow == nullptr)
		return;
	setMovable(false);

	// Menu
	auto menu = new QToolButton(this);
	menu->setText("Menu");
	menu->setIcon(Icon::get("application-menu"));
	menu->setPopupMode(QToolButton::InstantPopup);
	menu->setMenu(new MainMenu(spotify, settings, mainWindow));
	addWidget(menu);

	// Search
	search = addAction(Icon::get("edit-find"), "Search");
	search->setCheckable(true);
	QAction::connect(search, &QAction::triggered, mainWindow, &MainWindow::setSearchVisible);

	// Media controls
	addSeparator();
	auto previous = addAction(Icon::get("media-skip-backward"), "Previous");
	playPause = addAction(Icon::get("media-playback-start"), "Play");
	playPause->setShortcut(QKeySequence("Space"));
	QAction::connect(playPause, &QAction::triggered, [this](bool checked)
	{
		auto mainWindow = (MainWindow *) this->parent;
		auto current = mainWindow->getCurrentPlayback();
		current.isPlaying = !current.isPlaying;
		mainWindow->refreshed(current);
		auto status = playPause->iconText() == "Play"
			? this->spotify.pause()
			: this->spotify.resume();
		if (!status.isEmpty())
		{
			mainWindow->setStatus(QString("Failed to %1 playback: %2")
				.arg(playPause->iconText() == "Pause" ? "pause" : "resume")
				.arg(status), true);
		}
	});

	auto next = addAction(Icon::get("media-skip-forward"), "Next");
	QAction::connect(previous, &QAction::triggered, [this](bool checked)
	{
		auto mainWindow = (MainWindow *) this->parent;
		auto status = this->spotify.previous();
		if (!status.isEmpty())
			mainWindow->setStatus(QString("Failed to go to previous track: %1").arg(status), true);
		mainWindow->refresh();
	});

	QAction::connect(next, &QAction::triggered, [this](bool checked)
	{
		auto mainWindow = (MainWindow *) this->parent;
		auto status = this->spotify.next();
		if (!status.isEmpty())
			mainWindow->setStatus(QString("Failed to go to next track: %1").arg(status), true);
		mainWindow->refresh();
	});

	// Progress
	progress = new QSlider(this);
	progress->setOrientation(Qt::Orientation::Horizontal);
	QSlider::connect(progress, &QAbstractSlider::sliderReleased, this, [this]()
	{
		auto mainWindow = (MainWindow *) this->parent;
		auto status = this->spotify.seek(progress->value());
		if (!status.isEmpty())
			mainWindow->setStatus(QString("Failed to seek: %1").arg(status), true);

#ifdef USE_DBUS
		if (mainWindow->getMediaPlayer() != nullptr)
			mainWindow->getMediaPlayer()->stateUpdated();
#endif
	});

	addSeparator();
	addWidget(progress);
	addSeparator();
	position = new QLabel("0:00/0:00", this);
	if (settings.general.fixed_width_time)
		position->setFont(QFont("monospace"));
	addWidget(position);
	addSeparator();

	// Shuffle and repeat toggles
	shuffle = addAction(Icon::get("media-playlist-shuffle"), "Shuffle");
	shuffle->setCheckable(true);
	QAction::connect(shuffle, &QAction::triggered, [this](bool checked)
	{
		auto mainWindow = (MainWindow *) this->parent;
		auto current = mainWindow->getCurrentPlayback();
		current.shuffle = !current.shuffle;
		mainWindow->refreshed(current);
		auto status = this->spotify.setShuffle(checked);
		if (!status.isEmpty())
			mainWindow->setStatus(QString("Failed to toggle shuffle: %1").arg(status), true);
	});

	repeat = addAction(Icon::get("media-playlist-repeat"), "Repeat");
	repeat->setCheckable(true);
	QAction::connect(repeat, &QAction::triggered, [this](bool checked)
	{
		auto mainWindow = (MainWindow *) this->parent;
		auto current = mainWindow->getCurrentPlayback();
		auto repeatMode = QString(checked ? "context" : "off");
		current.repeat = repeatMode;
		mainWindow->refreshed(current);
		auto status = this->spotify.setRepeat(repeatMode);
		if (!status.isEmpty())
			mainWindow->setStatus(QString("Failed to toggle repeat: %1").arg(status), true);
	});

	// Volume
	volumeButton = new VolumeButton(settings, spotify, this);
	addWidget(volumeButton);
}