#pragma once

#include "lib/settings.hpp"
#include "mainwindow.hpp"
#include "menu/mainmenu.hpp"
#include "spotify/spotify.hpp"
#include "util/icon.hpp"
#include "volumebutton.hpp"

#include <QToolBar>
#include <QToolButton>

class MainToolBar: public QToolBar
{
Q_OBJECT

public:
	MainToolBar(spt::Spotify &spotify, lib::settings &settings, QWidget *parent);

	QAction *search = nullptr;
	QAction *playPause = nullptr;
	QWidget *parent = nullptr;
	QSlider *progress = nullptr;
	QLabel *position = nullptr;
	QAction *shuffle = nullptr;
	QAction *repeat = nullptr;
	VolumeButton *volumeButton = nullptr;

private:
	spt::Spotify &spotify;
	lib::settings &settings;
};
