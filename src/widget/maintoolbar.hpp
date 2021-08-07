#pragma once

#include "lib/settings.hpp"

#include "menu/mainmenu.hpp"
#include "spotify/spotify.hpp"
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
	MainToolBar(spt::Spotify &spotify, lib::settings &settings,
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
	void setBorderless(bool enabled);

protected:
	void resizeEvent(QResizeEvent *event) override;

private:
	void onPlayPause(bool checked);
	void onPrevious(bool checked);
	void onNext(bool checked);
	void onProgressReleased();
	void onShuffle(bool checked);
	void onRepeat(bool checked);
	void onMinimize(bool checked);

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
	QAction *close = nullptr;

	QSizeGrip *leftResize = nullptr;
	QSizeGrip *rightResize = nullptr;
};
