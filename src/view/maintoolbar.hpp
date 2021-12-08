#pragma once

#include "lib/settings.hpp"

#include "menu/mainmenu.hpp"
#include "util/icon.hpp"
#include "widget/volumebutton.hpp"
#include "widget/clickableslider.hpp"
#include "widget/dragarea.hpp"

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
	void setProgress(int current, int duration);
	void setProgress(const lib::spt::playback &playback);
	void setVolume(int volume);
	void setRepeat(lib::repeat_state state);
	void setShuffle(bool shuffle);
	void setPositionFont(const QFont &font);
	void setSearchChecked(bool checked);

protected:
	void resizeEvent(QResizeEvent *event) override;
	void showEvent(QShowEvent *event) override;

private:
	void updateSpacerSizes();

	void onPlayPause(bool checked);
	void onPrevious(bool checked);
	void onNext(bool checked);
	void onProgressReleased();
	void onShuffle(bool checked);
	void onRepeat(bool checked);
	void onMinimize(bool checked);

	QToolButton *menu;
	QAction *search;

	QAction *previous;
	QAction *playPause;
	QAction *next;

	QSlider *progress;
	QLabel *position;

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
