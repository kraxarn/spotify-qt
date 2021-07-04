#include "searchtab/searchtabtree.hpp"

SearchTab::SearchTabTree::SearchTabTree(const QStringList &headers, QWidget *parent)
	: QTreeWidget(parent)
{
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setRootIsDecorated(false);
	setAllColumnsShowFocus(true);

	setColumnCount(headers.length());
	setHeaderLabels(headers);
}
