#include "treeutils.hpp"

auto TreeUtils::itemWithChildren(QTreeWidget *tree,
	const QString &name, const QString &toolTip,
	const QStringList &childrenItems) -> QTreeWidgetItem *
{
	auto *item = new QTreeWidgetItem(tree, {
		name
	});
	item->setToolTip(0, toolTip);
	for (const auto &child : childrenItems)
	{
		item->addChild(new QTreeWidgetItem(item, {child}));
	}

	return item;
}

auto TreeUtils::itemWithNoChildren(QTreeWidget *tree, const QString &name,
	const QString &toolTip) -> QTreeWidgetItem *
{
	return itemWithChildren(tree, name, toolTip,
		QStringList());
}

auto TreeUtils::itemWithEmptyChild(QTreeWidget *tree, const QString &name,
	const QString &toolTip) -> QTreeWidgetItem *
{
	return itemWithChildren(tree, name, toolTip,
		QStringList(QString()));
}
