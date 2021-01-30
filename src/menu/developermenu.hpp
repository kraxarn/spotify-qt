#pragma once

#include "../dialog/deviceselectdialog.hpp"
#include "../dialog/openlinkdialog.hpp"
#include "../dialog/setupdialog.hpp"
#include "../dialog/trackscachedialog.hpp"
#include "../dialog/whatsnewdialog.hpp"
#include "../util/icon.hpp"
#include "lib/settings.hpp"

#include <QMenu>

class DeveloperMenu: public QMenu
{
Q_OBJECT

public:
	DeveloperMenu(lib::settings &settings, QWidget *parent);

private:
	lib::settings &settings;

	QMenu *dialogMenu();
	QMenu *infoMenu();
};
