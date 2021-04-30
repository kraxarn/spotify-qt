#pragma once

#include <QListWidgetItem>

template<typename T>
class ListWidgetItem: public QListWidgetItem
{
public:
	ListWidgetItem(const T &data, QListWidget *parent)
		: data(data),
		QListWidgetItem(parent)
	{
	}

	auto getData() const -> const T &
	{
		return data;
	}

private:
	const T &data;
};
