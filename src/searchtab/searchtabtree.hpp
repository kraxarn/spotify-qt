#pragma once

#include <QTreeWidget>
#include <QHeaderView>
#include <QResizeEvent>

namespace SearchTab
{
	class SearchTabTree: public QTreeWidget
	{
	protected:
		SearchTabTree(const QStringList &headers, QWidget *parent);

		void resizeEvent(QResizeEvent *event) override;
	};
}
