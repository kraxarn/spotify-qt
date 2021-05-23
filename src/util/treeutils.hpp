#pragma once

#include <QTreeWidget>
#include <QTreeWidgetItem>

class TreeUtils
{
public:
	/** Expandable QTreeWidgetItem with children */
	static auto itemWithChildren(QTreeWidget *tree, const QString &name,
		const QString &toolTip, const QStringList &childrenItems) -> QTreeWidgetItem *;

	/** Expandable QTreeWidgetItem without children */
	static auto itemWithNoChildren(QTreeWidget *tree, const QString &name,
		const QString &toolTip) -> QTreeWidgetItem *;

	/** Expandable QTreeWidgetItem with empty, placeholder, child */
	static auto itemWithEmptyChild(QTreeWidget *tree, const QString &name,
		const QString &toolTip) -> QTreeWidgetItem *;

	/** New QTreeWidgetItem with specified key and value */
	static auto item(QTreeWidget *tree, const QString &key,
		const QString &value) -> QTreeWidgetItem *;

	/** New QTreeWidgetItem with specified std::string key and value */
	static auto item(QTreeWidget *tree, const std::string &key,
		const std::string &value) -> QTreeWidgetItem *;

private:
	TreeUtils() = default;
};
