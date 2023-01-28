#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "dialog/settings.hpp"

#include <QMenu>

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
	const lib::http_client &httpClient;
	Dialog::Settings *settingsDialog = nullptr;

	void logOut(bool checked);

	void onOpenSettings(bool checked);
};
