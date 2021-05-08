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
	MainMenu(spt::Spotify &spotify, lib::settings &settings,
		const lib::http_client &httpClient, QWidget *parent);

private:
	spt::Spotify &spotify;
	lib::settings &settings;
	QAction *about;
	QMenu *deviceMenu;

	void refreshDevices();
	void deviceSelected(QAction *action);
	void logOut(bool checked);
	void checkForUpdate(const std::string &data);
};
