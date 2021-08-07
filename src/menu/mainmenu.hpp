#pragma once

#include "lib/spotify/api.hpp"

#include "menu/developermenu.hpp"
#include "util/menuutils.hpp"

#include <QMenu>
#include <QMessageBox>

class MainMenu: public QMenu
{
Q_OBJECT

public:
	MainMenu(lib::spt::api &spotify, lib::settings &settings, const lib::http_client &httpClient,
		lib::cache &cache, QWidget *parent);

private:
	lib::spt::api &spotify;
	lib::settings &settings;
	lib::cache &cache;
	QAction *about;
	QMenu *deviceMenu;

	void refreshDevices();
	void deviceSelected(QAction *action);
	void logOut(bool checked);
	void checkForUpdate(const std::string &data);
};
