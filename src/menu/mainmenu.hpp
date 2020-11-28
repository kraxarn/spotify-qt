#pragma once

#include "../mainwindow.hpp"
#include "../spotify/spotify.hpp"

#include <QMenu>
#include <QMessageBox>

class MainMenu: public QMenu
{
Q_OBJECT

public:
	MainMenu(spt::Spotify &spotify, lib::Settings &settings, QWidget *parent = nullptr);

private:
	QWidget *parent;
	spt::Spotify &spotify;
	lib::Settings &settings;
	QAction *about;
	QMenu *deviceMenu;
	void refreshDevices();
};
