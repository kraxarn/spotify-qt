#pragma once

#include "lib/settings.hpp"

#include "menu/mainmenu.hpp"
#include "util/icon.hpp"
#include "widget/volumebutton.hpp"
#include "widget/clickableslider.hpp"
#include "widget/dragarea.hpp"
#include "widget/position.hpp"

#include <QToolBar>
#include <QToolButton>
#include <QSizeGrip>

class MainToolBar: public QToolBar
{
Q_OBJECT

public:
	MainToolBar(lib::spt::api &spotify, lib::settings &settings,
		const lib::http_client &httpClient, lib::cache &cache, QWidget *parent);

	void showTitleBarButtons(bool show);
	void setPlaying(bool playing);
	void setProgress(const lib::spt::playback &playback);
	void setVolume(int volume);
	void setRepeat(lib::repeat_state state);
	auto toggleRepeat(const lib::spt::playback &playback) -> lib::repeat_state;
	void setShuffle(bool shuffle);
	void setSearchChecked(bool checked);
	void toggleActions(const lib::spt::playback &playback);

protected:
	void resizeEvent(QResizeEvent *event) override;
	void showEvent(QShowEvent *event) override;

private:
	void updateSpacerSizes();
	void setProgress(int current, int duration);
	auto isPlaying() const -> bool;

	auto addShortcutAction(const QString &iconName, const QString &title,
		const QKeySequence &shortcut) -> QAction *;

	auto getNextRepeatState(const lib::spt::playback &playback) -> lib::repeat_state;
	static auto getRepeatIcon(lib::repeat_state repeatState) -> QIcon;

	void onPlayPause(bool checked);
	void onPrevious(bool checked);
	void onNext(bool checked);
	void onProgressReleased();
	void onShuffle(bool checked);
	void onRepeat(bool checked);
	void onMinimize(bool checked);
	void onClose(bool checked);

	lib::repeat_state repeatState = lib::repeat_state::off;

	QToolButton *menu;
	QAction *search;

	QAction *previous;
	QAction *playPause;
	QAction *next;

	QSlider *progress;
	Position *position;

	QAction *shuffle;
	QAction *repeat;
	VolumeButton *volumeButton;

	lib::spt::api &spotify;
	lib::settings &settings;

	DragArea *leftSpacer;
	DragArea *rightSpacer;

	QAction *titleBarSeparator = nullptr;
	QAction *minimize = nullptr;
	QAction *close = nullptr;
};
