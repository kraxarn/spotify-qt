#pragma once

#include "dialog/settings.hpp"

class AppMenu
{
protected:
	explicit AppMenu(lib::settings &settings, const lib::http_client &httpClient,
		lib::cache &cache, QWidget *parent);

	void logOut();
	void openSettings();

private:
	QWidget *parent;
	lib::settings &settings;
	const lib::http_client &httpClient;
	lib::cache &cache;
};
