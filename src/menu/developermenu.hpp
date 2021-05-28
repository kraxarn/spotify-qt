#pragma once

#include "dialog/deviceselectdialog.hpp"
#include "dialog/openlinkdialog.hpp"
#include "dialog/setupdialog.hpp"
#include "dialog/trackscachedialog.hpp"
#include "dialog/whatsnewdialog.hpp"
#include "util/icon.hpp"
#include "lib/settings.hpp"
#include "lib/spotify/api.hpp"
#include "lib/httpclient.hpp"

#include <QMenu>
#include <QMainWindow>

class DeveloperMenu: public QMenu
{
Q_OBJECT

public:
	DeveloperMenu(lib::settings &settings, lib::spt::api &spotify,
		lib::cache &cache, const lib::http_client &httpClient, QWidget *parent);

private:
	lib::settings &settings;
	lib::spt::api &spotify;
	lib::cache &cache;
	const lib::http_client &httpClient;

	static void addMenuItem(QMenu *menu, const QString &text,
		const std::function<void()> &triggered);

	auto dialogMenu() -> QMenu *;
	auto infoMenu() -> QMenu *;
};
