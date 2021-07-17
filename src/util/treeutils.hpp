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

private:
	TreeUtils() = default;
};
