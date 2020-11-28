#pragma once

#include "../util/icon.hpp"
#include "lib/settings.hpp"

#include <QTreeWidget>
#include <QHeaderView>
#include <QMenu>

class ConfigView: public QTreeWidget
{
public:
	ConfigView(const lib::Settings &settings, QWidget *parent);

private:
	void menu(const QPoint &pos);
};
