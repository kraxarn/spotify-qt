#pragma once

#include <QTreeWidget>
#include <QHeaderView>
#include <QResizeEvent>

namespace Search
{
	class SearchTabTree: public QTreeWidget
	{
	Q_OBJECT

	protected:
		SearchTabTree(const QStringList &headers, QWidget *parent);

		virtual void resizeEvent(QResizeEvent *event) override;
	};
}
