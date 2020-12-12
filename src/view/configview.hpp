#pragma once

#include "../settings.hpp"
#include "../util/icon.hpp"

#include <QTreeWidget>
#include <QHeaderView>
#include <QMenu>

class ConfigView: public QTreeWidget
{
public:
	ConfigView(const Settings &settings, QWidget *parent);

private:
	const Settings &settings;

	void menu(const QPoint &pos);
};
