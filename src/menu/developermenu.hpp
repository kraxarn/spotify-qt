#pragma once

#include "dialog/deviceselect.hpp"
#include "dialog/openlink.hpp"
#include "dialog/setup.hpp"
#include "dialog/trackscachedialog.hpp"
#include "dialog/whatsnew.hpp"
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

	QMenu *dialogMenu;

	static void addMenuItem(QMenu *menu, const QString &text,
		const std::function<void()> &triggered);

	auto getDialogMenu() -> QMenu *;
	auto infoMenu() -> QMenu *;
	auto crashMenu() -> QMenu *;
	auto statusMenu() -> QMenu *;

	void onDialogMenuAboutToShow();
};
