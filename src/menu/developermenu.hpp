#pragma once

#include "dialog/deviceselectdialog.hpp"
#include "dialog/openlinkdialog.hpp"
#include "dialog/setupdialog.hpp"
#include "dialog/trackscachedialog.hpp"
#include "dialog/whatsnewdialog.hpp"
#include "util/icon.hpp"
#include "lib/settings.hpp"

#include <QMenu>
#include <QMainWindow>

class DeveloperMenu: public QMenu
{
Q_OBJECT

public:
	DeveloperMenu(lib::settings &settings, lib::spt::api &spotify,
		lib::cache &cache, QWidget *parent);

private:
	lib::settings &settings;
	lib::spt::api &spotify;
	lib::cache &cache;

	static void addMenuItem(QMenu *menu, const QString &text,
		const std::function<void()> &triggered);

	QMenu *dialogMenu();
	QMenu *infoMenu();
};
