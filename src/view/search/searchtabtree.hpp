#pragma once

#include <QTreeWidget>
#include <QHeaderView>
#include <QResizeEvent>

namespace View
{
	namespace Search
	{
		class SearchTabTree: public QTreeWidget
		{
		protected:
			SearchTabTree(const QStringList &headers, QWidget *parent);

			void resizeEvent(QResizeEvent *event) override;
		};
	}
}
