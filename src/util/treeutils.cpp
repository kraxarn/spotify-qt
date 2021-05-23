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

auto TreeUtils::itemWithChildren(QTreeWidget *tree, const QString &name,
	const QString &toolTip) -> QTreeWidgetItem *
{
	return itemWithChildren(tree, name, toolTip, QStringList());
}

auto TreeUtils::item(QTreeWidget *tree, const QString &key,
	const QString &value) -> QTreeWidgetItem *
{
	return new QTreeWidgetItem(tree, {
		key, value
	});
}

auto TreeUtils::item(QTreeWidget *tree, const std::string &key,
	const std::string &value) -> QTreeWidgetItem *
{
	return treeItem(tree, QString::fromStdString(key), QString::fromStdString(value));
}
