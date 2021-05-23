#pragma once

#include <QTreeWidget>
#include <QTreeWidgetItem>

class TreeUtils
{
public:
	static auto itemWithChildren(QTreeWidget *tree, const QString &name,
		const QString &toolTip, const QStringList &childrenItems) -> QTreeWidgetItem *;

	static auto itemWithChildren(QTreeWidget *tree, const QString &name,
		const QString &toolTip) -> QTreeWidgetItem *;

	static auto item(QTreeWidget *tree, const QString &key,
		const QString &value) -> QTreeWidgetItem *;

	static auto item(QTreeWidget *tree, const std::string &key,
		const std::string &value) -> QTreeWidgetItem *;

private:
	TreeUtils() = default;
};
