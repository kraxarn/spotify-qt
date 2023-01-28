#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "menu/appmenu.hpp"

#include <QMenuBar>

class MainMenuBar: public QMenuBar, AppMenu
{
public:
	MainMenuBar(lib::spt::api &spotify, lib::settings &settings,
		const lib::http_client &httpClient, lib::cache &cache, QWidget *parent);

private:
	void onOpenPreferences(bool checked);
	void onOpenAbout(bool checked);
	void onLogOut(bool checked);
};
