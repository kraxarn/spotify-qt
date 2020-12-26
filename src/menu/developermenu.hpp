#pragma once

#include "../dialog/deviceselectdialog.hpp"
#include "../dialog/openlinkdialog.hpp"
#include "../dialog/setupdialog.hpp"
#include "../dialog/whatsnewdialog.hpp"
#include "../settings.hpp"
#include "../util/icon.hpp"

#include <QMenu>

class DeveloperMenu: public QMenu
{
Q_OBJECT

public:
	DeveloperMenu(Settings &settings, QWidget *parent);

private:
	Settings &settings;

	QMenu *dialogMenu();
};
