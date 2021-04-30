#pragma once

#include <QTreeWidgetItem>

template<typename T>
class TreeWidgetItem: public QTreeWidgetItem
{
public:
	TreeWidgetItem(const T &data, QTreeWidget *parent)
		: data(data),
		QTreeWidgetItem(parent)
	{
	}

	auto getData() const -> const T &
	{
		return data;
	}

private:
	const T &data;
};
