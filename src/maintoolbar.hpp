#pragma once

#include "lib/settings.hpp"
#include "mainwindow.hpp"
#include "menu/mainmenu.hpp"
#include "spotify/spotify.hpp"
#include "util/icon.hpp"
#include "volumebutton.hpp"
#include "view/clickableslider.hpp"
#include "dragarea.hpp"

#include <QToolBar>
#include <QToolButton>

class MainToolBar: public QToolBar
{
Q_OBJECT

public:
	MainToolBar(spt::Spotify &spotify, lib::settings &settings, QWidget *parent);

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

private:
	QAction *search = nullptr;
	QAction *playPause = nullptr;
	QWidget *parent = nullptr;
	QSlider *progress = nullptr;
	QLabel *position = nullptr;
	QAction *shuffle = nullptr;
	QAction *repeat = nullptr;
	VolumeButton *volumeButton = nullptr;

	spt::Spotify &spotify;
	lib::settings &settings;

	DragArea *leftSpacer = nullptr;
	DragArea *rightSpacer = nullptr;

	QAction *titleBarSeparator = nullptr;
	QAction *minimize = nullptr;
	QAction *maximize = nullptr;
	QAction *close = nullptr;
};
