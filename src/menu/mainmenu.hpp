#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "menu/appmenu.hpp"

#include <QMenu>

class MainMenu: public QMenu, AppMenu
{
Q_OBJECT

public:
	MainMenu(lib::spt::api &spotify, lib::settings &settings, const lib::http_client &httpClient,
		lib::cache &cache, QWidget *parent);

private:
	void onLogOut(bool checked);
	void onOpenSettings(bool checked);
};
