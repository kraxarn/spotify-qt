#include "view/maintoolbar.hpp"
#include "mainwindow.hpp"
#include "util/shortcut.hpp"
#include "util/appconfig.hpp"

MainToolBar::MainToolBar(lib::spt::api &spotify, lib::settings &settings,
	const lib::http_client &httpClient, lib::cache &cache, QWidget *parent)
	: QToolBar("Media controls", parent),
	spotify(spotify),
	settings(settings)
{
	auto *mainWindow = MainWindow::find(parent);
	if (mainWindow == nullptr)
	{
		return;
	}
	setMovable(false);

	// Menu
	menu = new QToolButton(this);
	menu->setText("Menu");
	menu->setIcon(Icon::get("application-menu"));
	menu->setPopupMode(QToolButton::InstantPopup);
	menu->setVisible(!AppConfig::useNativeMenuBar());
	menu->setMenu(new MainMenu(spotify, settings, httpClient, cache, this));

	// Search
	search = addAction(Icon::get(QStringLiteral("edit-find")),
		QString("Search (%1)").arg(QKeySequence(QKeySequence::Find)
			.toString(QKeySequence::NativeText)));
	search->setCheckable(true);
	search->setShortcut(QKeySequence::Find);
	QAction::connect(search, &QAction::triggered, mainWindow, &MainWindow::setSearchVisible);

	// Media controls
	auto *mediaControlsSeparator = addSeparator();
	previous = addShortcutAction(QStringLiteral("media-skip-backward"),
		QStringLiteral("Previous"), Shortcut::previousTrack());
	QAction::connect(previous, &QAction::triggered,
		this, &MainToolBar::onPrevious);

	playPause = addShortcutAction(QStringLiteral("media-playback-start"),
		QStringLiteral("Play"), Shortcut::playPause());
	QAction::connect(playPause, &QAction::triggered,
		this, &MainToolBar::onPlayPause);

	next = addShortcutAction(QStringLiteral("media-skip-forward"),
		QStringLiteral("Next"), Shortcut::nextTrack());
	QAction::connect(next, &QAction::triggered,
		this, &MainToolBar::onNext);

	addSeparator();
	leftSpacer = new DragArea(mainWindow, this);
	addWidget(leftSpacer);

	// Progress
	progress = new ClickableSlider(Qt::Horizontal, this);
	QSlider::connect(progress, &QAbstractSlider::sliderReleased,
		this, &MainToolBar::onProgressReleased);

	addWidget(progress);
	addSeparator();
	position = new Position(this);
	addWidget(position);

	rightSpacer = new DragArea(mainWindow, this);
	addWidget(rightSpacer);
	addSeparator();

	// Shuffle and repeat toggles
	shuffle = addShortcutAction(QStringLiteral("media-playlist-shuffle"),
		QStringLiteral("Shuffle"), Shortcut::shuffle());
	shuffle->setCheckable(true);
	QAction::connect(shuffle, &QAction::triggered,
		this, &MainToolBar::onShuffle);

	repeat = addShortcutAction(QStringLiteral("media-playlist-repeat"),
		QStringLiteral("Repeat"), Shortcut::repeat());
	repeat->setCheckable(true);
	QAction::connect(repeat, &QAction::triggered,
		this, &MainToolBar::onRepeat);

	// Volume
	volumeButton = new VolumeButton(settings, spotify, this);
	addWidget(volumeButton);

	// Title bar buttons
	titleBarSeparator = addSeparator();

	minimize = new QAction(Icon::get("window-minimize"),
		QStringLiteral("Minimize"), this);

	QAction::connect(minimize, &QAction::triggered,
		this, &MainToolBar::onMinimize);

	close = new QAction(Icon::get("window-close"),
		QStringLiteral("Close"), this);

	QAction::connect(close, &QAction::triggered, this, &MainToolBar::onClose);

	if (settings.qt().mirror_title_bar)
	{
		insertAction(mediaControlsSeparator, minimize);
		insertAction(minimize, close);

		addAction(search);
		addWidget(menu);
	}
	else
	{
		insertAction(mediaControlsSeparator, search);
		insertWidget(search, menu);

		addAction(minimize);
		addAction(close);
	}
}

void MainToolBar::resizeEvent(QResizeEvent */*event*/)
{
	updateSpacerSizes();
}

void MainToolBar::showEvent(QShowEvent */*event*/)
{
	showTitleBarButtons(!settings.qt().system_title_bar);
	updateSpacerSizes();
}

void MainToolBar::showTitleBarButtons(bool show)
{
	titleBarSeparator->setVisible(show);

	minimize->setVisible(show);
	close->setVisible(show);

	leftSpacer->setVisible(show);
	rightSpacer->setVisible(show);
}

void MainToolBar::updateSpacerSizes()
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

auto MainToolBar::addShortcutAction(const QString &iconName, const QString &title,
	const QKeySequence &shortcut) -> QAction *
{
	auto *action = addAction(Icon::get(iconName), QStringLiteral("%1 (%2)")
		.arg(title, shortcut.toString(QKeySequence::NativeText)));

	action->setShortcut(shortcut);
	return action;
}

auto MainToolBar::getNextRepeatState(const lib::spt::playback &playback) -> lib::repeat_state
{
	switch (repeatState)
	{
		case lib::repeat_state::off:
			return playback.is_allowed(lib::player_action::toggling_repeat_context)
				? lib::repeat_state::context
				: lib::repeat_state::track;

		case lib::repeat_state::context:
			return playback.is_allowed(lib::player_action::toggling_repeat_track)
				? lib::repeat_state::track
				: lib::repeat_state::off;

		default:
			return lib::repeat_state::off;
	}
}

auto MainToolBar::getRepeatIcon(lib::repeat_state repeatState) -> QIcon
{
	if (repeatState == lib::repeat_state::track)
	{
		return Icon::get(QStringLiteral("media-playlist-repeat-song"));
	}

	return Icon::get(QStringLiteral("media-playlist-repeat"));
}

void MainToolBar::setPlaying(bool playing)
{
	playPause->setIcon(Icon::get(playing
		? QStringLiteral("media-playback-pause")
		: QStringLiteral("media-playback-start")));

	playPause->setText(QString("%1 (%2)")
		.arg(playing ? QStringLiteral("Pause") : QStringLiteral("Play"),
			Shortcut::playPause().toString(QKeySequence::NativeText)));
}

void MainToolBar::setProgress(int current, int duration)
{
	position->setCurrent(current);
	position->setTotal(duration);

	progress->setValue(current);
	progress->setMaximum(duration);
}

void MainToolBar::setProgress(const lib::spt::playback &playback)
{
	setProgress(playback.progress_ms, playback.item.duration);
}

auto MainToolBar::isPlaying() const -> bool
{
	return playPause->text().startsWith(QStringLiteral("Pause"));
}

void MainToolBar::toggleActions(const lib::spt::playback &playback)
{
	if (!playback.is_allowed(lib::player_action::interrupting_playback))
	{
		playPause->setEnabled(false);
	}
	else
	{
		const auto buttonAction = isPlaying()
			? lib::player_action::pausing
			: lib::player_action::resuming;

		playPause->setEnabled(playback.is_allowed(buttonAction));
	}

	progress->setEnabled(playback.is_allowed(lib::player_action::seeking));
	next->setEnabled(playback.is_allowed(lib::player_action::skipping_next));
	previous->setEnabled(playback.is_allowed(lib::player_action::skipping_prev));

	repeat->setEnabled(playback.is_allowed(lib::player_action::toggling_repeat_context)
		|| playback.is_allowed(lib::player_action::toggling_repeat_track));

	shuffle->setEnabled(playback.is_allowed(lib::player_action::toggling_shuffle));
}

void MainToolBar::setVolume(int volume)
{
	volumeButton->setVolume(volume);
}

void MainToolBar::setRepeat(lib::repeat_state state)
{
	repeat->setChecked(state != lib::repeat_state::off);
	repeat->setIcon(getRepeatIcon(state));
	repeatState = state;
}

auto MainToolBar::toggleRepeat(const lib::spt::playback &playback) -> lib::repeat_state
{
	const auto newState = getNextRepeatState(playback);
	setRepeat(newState);
	return newState;
}

void MainToolBar::setShuffle(bool value)
{
	shuffle->setChecked(value);
}

void MainToolBar::setSearchChecked(bool checked)
{
	search->setChecked(checked);
}

void MainToolBar::onPlayPause(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	auto &current = mainWindow->getCurrentPlayback();

#ifdef USE_DBUS
	auto *mediaPlayer = mainWindow->getMediaPlayer();
	if (mediaPlayer != nullptr)
	{
		mediaPlayer->stateUpdated();
	}
#endif

	current.is_playing = !current.is_playing;
	mainWindow->refreshed(current);

	auto callback = [this](const std::string &status)
	{
		if (status.empty())
		{
			return;
		}

		StatusMessage::error(QString("Failed to %1 playback: %2")
			.arg(isPlaying() ? QStringLiteral("pause") : QStringLiteral("resume"),
				QString::fromStdString(status)));
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
		if (!status.empty())
		{
			StatusMessage::error(QString("Failed to go to previous track: %1")
				.arg(QString::fromStdString(status)));
			return;
		}

		auto *mainWindow = MainWindow::find(this->parentWidget());
		mainWindow->refresh();
	});
}

void MainToolBar::onNext(bool /*checked*/)
{
	spotify.next([this](const std::string &status)
	{
		if (!status.empty())
		{
			StatusMessage::error(QString("Failed to go to next track: %1")
				.arg(QString::fromStdString(status)));
			return;
		}

		auto *mainWindow = MainWindow::find(this->parentWidget());
		mainWindow->refresh();
	});
}

void MainToolBar::onProgressReleased()
{
	spotify.seek(progress->value(), [this](const std::string &status)
	{
		if (!status.empty())
		{
			StatusMessage::error(QString("Failed to seek: %1")
				.arg(QString::fromStdString(status)));
			return;
		}

#ifdef USE_DBUS
		auto *mainWindow = MainWindow::find(this->parentWidget());
		auto *mediaPlayer = mainWindow->getMediaPlayer();
		if (mediaPlayer != nullptr)
		{
			mediaPlayer->stateUpdated();
			mediaPlayer->seeked(progress->value());
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

	spotify.set_shuffle(checked, [](const std::string &status)
	{
		if (status.empty())
		{
			return;
		}

		StatusMessage::error(QString("Failed to toggle shuffle: %1")
			.arg(QString::fromStdString(status)));
	});
}

void MainToolBar::onRepeat(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	auto &current = mainWindow->getCurrentPlayback();

	const auto repeatMode = toggleRepeat(current);
	current.repeat = repeatMode;
	mainWindow->refreshed(current);

	spotify.set_repeat(repeatMode, [](const std::string &status)
	{
		if (status.empty())
		{
			return;
		}

		StatusMessage::error(QString("Failed to toggle repeat: %1")
			.arg(QString::fromStdString(status)));
	});
}

void MainToolBar::onMinimize(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->minimize();
}

void MainToolBar::onClose(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	if (settings.general.close_to_tray && mainWindow->getTrayIcon() != nullptr)
	{
		mainWindow->setVisible(false);
	}
	else
	{
		QCoreApplication::quit();
	}
}
