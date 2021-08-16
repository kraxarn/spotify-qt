#include "view/maintoolbar.hpp"
#include "mainwindow.hpp"

MainToolBar::MainToolBar(lib::spt::api &spotify, lib::settings &settings,
	const lib::http_client &httpClient, lib::cache &cache, QWidget *parent)
	: parent(parent),
	spotify(spotify),
	settings(settings),
	QToolBar("Media controls", parent)
{
	auto *mainWindow = MainWindow::find(parent);
	if (mainWindow == nullptr)
	{
		return;
	}
	setMovable(false);

	// Menu
	auto *menu = new QToolButton(this);
	menu->setText("Menu");
	menu->setIcon(Icon::get("application-menu"));
	menu->setPopupMode(QToolButton::InstantPopup);
	menu->setMenu(new MainMenu(spotify, settings,
		httpClient, cache, mainWindow));
	addWidget(menu);

	// Search
	search = addAction(Icon::get("edit-find"), "Search");
	search->setCheckable(true);
	QAction::connect(search, &QAction::triggered, mainWindow, &MainWindow::setSearchVisible);

	// Media controls
	addSeparator();
	auto *previous = addAction(Icon::get("media-skip-backward"), "Previous");
	QAction::connect(previous, &QAction::triggered,
		this, &MainToolBar::onPrevious);

	playPause = addAction(Icon::get("media-playback-start"), "Play");
	playPause->setShortcut(QKeySequence("Space"));
	QAction::connect(playPause, &QAction::triggered,
		this, &MainToolBar::onPlayPause);

	auto *next = addAction(Icon::get("media-skip-forward"), "Next");
	QAction::connect(next, &QAction::triggered,
		this, &MainToolBar::onNext);

	addSeparator();
	leftSpacer = new DragArea(this);
	addWidget(leftSpacer);

	// Progress
	progress = new ClickableSlider(Qt::Horizontal, this);
	QSlider::connect(progress, &QAbstractSlider::sliderReleased,
		this, &MainToolBar::onProgressReleased);

	addWidget(progress);
	addSeparator();
	position = new QLabel("0:00/0:00", this);
	if (settings.general.fixed_width_time)
	{
		position->setFont(QFont("monospace"));
	}
	addWidget(position);

	rightSpacer = new DragArea(this);
	addWidget(rightSpacer);
	addSeparator();

	// Shuffle and repeat toggles
	shuffle = addAction(Icon::get("media-playlist-shuffle"), "Shuffle");
	shuffle->setCheckable(true);
	QAction::connect(shuffle, &QAction::triggered,
		this, &MainToolBar::onShuffle);

	repeat = addAction(Icon::get("media-playlist-repeat"), "Repeat");
	repeat->setCheckable(true);
	QAction::connect(repeat, &QAction::triggered,
		this, &MainToolBar::onRepeat);

	// Volume
	volumeButton = new VolumeButton(settings, spotify, this);
	addWidget(volumeButton);

	// Title bar buttons
	titleBarSeparator = addSeparator();
	minimize = addAction(Icon::get("window-minimize-symbolic"), "Minimize");
	QAction::connect(minimize, &QAction::triggered,
		this, &MainToolBar::onMinimize);

	close = addAction(Icon::get("window-close-symbolic"), "Close");
	QAction::connect(close, &QAction::triggered, &QCoreApplication::quit);

	showTitleBarButtons(!settings.qt_const().system_title_bar);
}

void MainToolBar::resizeEvent(QResizeEvent */*event*/)
{
	if (!leftSpacer->isVisible()
		&& !rightSpacer->isVisible())
	{
		return;
	}

	const auto width = size().width();
	const auto spacerWidth = width / 6;

	leftSpacer->setMinimumWidth(spacerWidth);
	rightSpacer->setMinimumWidth(spacerWidth);
}

void MainToolBar::showTitleBarButtons(bool show)
{
	titleBarSeparator->setVisible(show);

	minimize->setVisible(show);
	close->setVisible(show);

	leftSpacer->setVisible(show);
	rightSpacer->setVisible(show);
}

void MainToolBar::setPlaying(bool playing)
{
	playPause->setIcon(Icon::get(playing
		? "media-playback-pause"
		: "media-playback-start"));

	playPause->setText(playing ? "Pause" : "Play");
}

void MainToolBar::setProgress(int current, int duration)
{
	position->setText(QString("%1/%2")
		.arg(QString::fromStdString(lib::fmt::time(current)),
			QString::fromStdString(lib::fmt::time(duration))));

	progress->setValue(current);
	progress->setMaximum(duration);
}

void MainToolBar::setProgress(const lib::spt::playback &playback)
{
	setProgress(playback.progress_ms, playback.item.duration);
}

void MainToolBar::setVolume(int volume)
{
	volumeButton->setVolume(volume);
}

void MainToolBar::setRepeat(lib::repeat_state state)
{
	repeat->setChecked(state != lib::repeat_state::off);
}

void MainToolBar::setShuffle(bool value)
{
	shuffle->setChecked(value);
}

void MainToolBar::setPositionFont(const QFont &font)
{
	position->setFont(font);
}

void MainToolBar::setSearchChecked(bool checked)
{
	search->setChecked(checked);
}

void MainToolBar::onPlayPause(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	auto &current = mainWindow->getCurrentPlayback();

	current.is_playing = !current.is_playing;
	mainWindow->refreshed(current);

	auto callback = [this, mainWindow](const std::string &status)
	{
		if (status.empty())
		{
			return;
		}

		mainWindow->status(lib::fmt::format("Failed to {} playback: {}",
			this->playPause->iconText() == "Pause" ? "pause" : "resume",
			status), true);
	};

	if (current.is_playing)
	{
		spotify.resume(callback);
	}
	else
	{
		spotify.pause(callback);
	}
}

void MainToolBar::onPrevious(bool /*checked*/)
{
	this->spotify.previous([this](const std::string &status)
	{
		auto *mainWindow = MainWindow::find(this->parentWidget());
		if (!status.empty())
		{
			mainWindow->status(lib::fmt::format("Failed to go to previous track: {}",
				status), true);
		}
		mainWindow->refresh();
	});
}

void MainToolBar::onNext(bool /*checked*/)
{
	spotify.next([this](const std::string &status)
	{
		auto *mainWindow = MainWindow::find(this->parentWidget());
		if (!status.empty())
		{
			mainWindow->status(lib::fmt::format("Failed to go to next track: {}",
				status), true);
		}
		mainWindow->refresh();
	});
}

void MainToolBar::onProgressReleased()
{
	spotify.seek(progress->value(), [this](const std::string &status)
	{
		auto *mainWindow = MainWindow::find(this->parentWidget());

		if (!status.empty())
		{
			mainWindow->status(lib::fmt::format("Failed to seek: {}",
				status), true);
		}

#ifdef USE_DBUS
		if (mainWindow->getMediaPlayer() != nullptr)
		{
			mainWindow->getMediaPlayer()->stateUpdated();
		}
#endif
	});
}

void MainToolBar::onShuffle(bool checked)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	auto &current = mainWindow->getCurrentPlayback();

	current.shuffle = !current.shuffle;
	mainWindow->refreshed(current);

	spotify.set_shuffle(checked, [mainWindow](const std::string &status)
	{
		if (!status.empty())
		{
			mainWindow->status(lib::fmt::format("Failed to toggle shuffle: {}",
				status), true);
		}
	});
}

void MainToolBar::onRepeat(bool checked)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	auto &current = mainWindow->getCurrentPlayback();
	auto repeatMode = checked
		? lib::repeat_state::context
		: lib::repeat_state::off;

	current.repeat = repeatMode;
	mainWindow->refreshed(current);

	spotify.set_repeat(repeatMode, [mainWindow](const std::string &status)
	{
		if (!status.empty())
		{
			mainWindow->status(lib::fmt::format("Failed to toggle repeat: {}",
				status), true);
		}
	});
}

void MainToolBar::onMinimize(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->minimize();
}
