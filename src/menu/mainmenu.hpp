#pragma once

#include "../mainwindow.hpp"
#include "../spotify/spotify.hpp"
#include "developermenu.hpp"

#include <QMenu>
#include <QMessageBox>

class MainMenu: public QMenu
{
Q_OBJECT

public:
	MainMenu(spt::Spotify &spotify, Settings &settings, QWidget *parent = nullptr);

	static bool showDebugMenu;

private:
	spt::Spotify &spotify;
	Settings &settings;
	QAction *about;
	QMenu *deviceMenu;
	void refreshDevices();
};
