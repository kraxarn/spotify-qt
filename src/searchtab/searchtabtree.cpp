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

void SearchTab::SearchTabTree::resizeEvent(QResizeEvent *event)
{
	QAbstractItemView::resizeEvent(event);

	auto count = header()->count() - header()->hiddenSectionCount();
	auto totalWidth = event->size().width();
	auto width = totalWidth / count;

	for (auto i = 0; i < header()->count(); i++)
	{
		if (header()->isSectionHidden(i))
		{
			continue;
		}
		header()->resizeSection(i, width);
	}
}
