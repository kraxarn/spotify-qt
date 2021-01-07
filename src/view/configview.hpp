#pragma once

#include "lib/settings.hpp"
#include "../util/icon.hpp"

#include <QTreeWidget>
#include <QHeaderView>
#include <QMenu>

class ConfigView: public QTreeWidget
{
public:
	ConfigView(const lib::settings &settings, QWidget *parent);

private:
	const lib::settings &settings;

	void menu(const QPoint &pos);
};
