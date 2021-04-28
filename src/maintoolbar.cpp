#include "maintoolbar.hpp"

MainToolBar::MainToolBar(spt::Spotify &spotify, lib::settings &settings, QWidget *parent)
	: parent(parent),
	spotify(spotify),
	settings(settings),
	QToolBar("Media controls", parent)
{
	auto *mainWindow = dynamic_cast<MainWindow *>(parent);
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
	menu->setMenu(new MainMenu(spotify, settings, mainWindow));
	addWidget(menu);

	// Search
	search = addAction(Icon::get("edit-find"), "Search");
	search->setCheckable(true);
	QAction::connect(search, &QAction::triggered, mainWindow, &MainWindow::setSearchVisible);

	// Media controls
	addSeparator();
	auto *previous = addAction(Icon::get("media-skip-backward"), "Previous");
	playPause = addAction(Icon::get("media-playback-start"), "Play");
	playPause->setShortcut(QKeySequence("Space"));
	QAction::connect(playPause, &QAction::triggered, [this, mainWindow](bool /*checked*/)
	{
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
			this->spotify.resume(callback);
		}
		else
		{
			this->spotify.pause(callback);
		}
	});

	auto *next = addAction(Icon::get("media-skip-forward"), "Next");
	QAction::connect(previous, &QAction::triggered, [this, mainWindow](bool /*checked*/)
	{
		this->spotify.previous([mainWindow](const std::string &status)
		{
			if (!status.empty())
			{
				mainWindow->status(lib::fmt::format("Failed to go to previous track: {}",
					status), true);
			}
			mainWindow->refresh();
		});
	});

	QAction::connect(next, &QAction::triggered, [this, mainWindow](bool /*checked*/)
	{
		this->spotify.next([mainWindow](const std::string &status)
		{
			if (!status.empty())
			{
				mainWindow->status(lib::fmt::format("Failed to go to next track: {}",
					status), true);
			}
			mainWindow->refresh();
		});
	});

	leftSpacer = new DragArea(this);
	addWidget(leftSpacer);

	// Progress
	progress = new ClickableSlider(Qt::Horizontal, this);
	QSlider::connect(progress, &QAbstractSlider::sliderReleased,
		this, [this, mainWindow]()
		{
			this->spotify.seek(progress->value(),
				[mainWindow](const std::string &status)
				{
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
		});

	addSeparator();
	addWidget(progress);
	addSeparator();
	position = new QLabel("0:00/0:00", this);
	if (settings.general.fixed_width_time)
	{
		position->setFont(QFont("monospace"));
	}
	addWidget(position);
	addSeparator();

	rightSpacer = new DragArea(this);
	addWidget(rightSpacer);

	// Shuffle and repeat toggles
	shuffle = addAction(Icon::get("media-playlist-shuffle"), "Shuffle");
	shuffle->setCheckable(true);
	QAction::connect(shuffle, &QAction::triggered, [this, mainWindow](bool checked)
	{
		auto &current = mainWindow->getCurrentPlayback();
		current.shuffle = !current.shuffle;
		mainWindow->refreshed(current);

		this->spotify.set_shuffle(checked, [mainWindow](const std::string &status)
		{
			if (!status.empty())
			{
				mainWindow->status(lib::fmt::format("Failed to toggle shuffle: {}",
					status), true);
			}
		});
	});

	repeat = addAction(Icon::get("media-playlist-repeat"), "Repeat");
	repeat->setCheckable(true);
	QAction::connect(repeat, &QAction::triggered, [this, mainWindow](bool checked)
	{
		auto &current = mainWindow->getCurrentPlayback();
		auto repeatMode = checked
			? lib::repeat_state::context
			: lib::repeat_state::off;
		current.repeat = repeatMode;
		mainWindow->refreshed(current);

		this->spotify.set_repeat(repeatMode, [mainWindow](const std::string &status)
		{
			if (!status.empty())
			{
				mainWindow->status(lib::fmt::format("Failed to toggle repeat: {}",
					status), true);
			}
		});
	});

	// Volume
	volumeButton = new VolumeButton(settings, spotify, this);
	addWidget(volumeButton);

	// Title bar buttons
	titleBarSeparator = addSeparator();
	minimize = addAction(Icon::get("window-minimize-symbolic"), "Minimize");
	QAction::connect(minimize, &QAction::triggered, [mainWindow](bool /*checked*/)
	{
		emit mainWindow->showMinimized();
	});

	maximize = addAction(Icon::get("window-maximize-symbolic"), "Maximise");
	QAction::connect(maximize, &QAction::triggered, [mainWindow](bool /*checked*/)
	{
		if (mainWindow->isMaximized())
		{
			mainWindow->resize(1280, 720);
		}
		else
		{
			emit mainWindow->showMaximized();
		}
	});

	close = addAction(Icon::get("window-close-symbolic"), "Close");
	QAction::connect(close, &QAction::triggered, &QCoreApplication::quit);

	showTitleBarButtons(!settings.qt_const().system_title_bar);
}

void MainToolBar::resizeEvent(QResizeEvent */*event*/)
{
	const auto width = size().width();
	const auto spacerWidth = width / 6;

	leftSpacer->setMinimumWidth(spacerWidth);
	rightSpacer->setMinimumWidth(spacerWidth);
}

void MainToolBar::showTitleBarButtons(bool show)
{
	titleBarSeparator->setVisible(show);

	minimize->setVisible(show);
	maximize->setVisible(show);
	close->setVisible(show);

	leftSpacer->setVisible(show);
	rightSpacer->setVisible(show);
}
