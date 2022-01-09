#pragma once

#include "lib/settings.hpp"
#include "util/icon.hpp"
#include "util/url.hpp"

#include <QTreeWidget>
#include <QHeaderView>
#include <QMenu>
#include <QCheckBox>
#include <QClipboard>

class ConfigView: public QWidget
{
public:
	ConfigView(const lib::settings &settings, QWidget *parent);

protected:
	void showEvent(QShowEvent *event) override;

private:
	QTreeWidget *tree;
	QHBoxLayout *footer;

	const lib::settings &settings;

	/**
	 * Settings, with private information removed, as JSON
	 */
	auto safeSettings() -> QString;

	void reload();

	void onMenuRequested(const QPoint &pos);
	void onCopyToClipboard(bool checked);
};
