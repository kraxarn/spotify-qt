#pragma once

#include "lib/settings.hpp"
#include "../util/icon.hpp"

#include <QTreeWidget>
#include <QHeaderView>
#include <QMenu>
#include <QCheckBox>

class ConfigView: public QTreeWidget
{
public:
	ConfigView(const lib::settings &settings, QWidget *parent);

private:
	const lib::settings &settings;

	void showEvent(QShowEvent *event) override;
	void menu(const QPoint &pos);
	void reload();
};
