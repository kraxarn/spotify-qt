#pragma once

#include <QTreeWidget>

namespace SearchTab
{
	class SearchTabTree: public QTreeWidget
	{
	protected:
		SearchTabTree(const QStringList &headers, QWidget *parent);
	};
}
