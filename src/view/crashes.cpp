#include "view/crashes.hpp"

View::Crashes::Crashes(lib::cache &cache, QWidget *parent)
	: cache(cache),
	QWidget(parent)
{
	auto *layout = new QHBoxLayout(this);

	constexpr int listWidth = 140;
	list = new QListWidget(this);
	list->setFixedWidth(listWidth);
	layout->addWidget(list);

	log = new QTextEdit(this);
	log->setFontFamily("monospace");
	layout->addWidget(log, 1);

	QListWidget::connect(list, &QListWidget::currentItemChanged,
		this, &View::Crashes::logItemChanged);
}

void View::Crashes::showEvent(QShowEvent */*event*/)
{
	list->clear();
	for (const auto &crash: cache.get_all_crashes())
	{
		list->addItem(new ListItem::Crash(crash, list));
	}

	list->sortItems(Qt::DescendingOrder);
	if (list->count() > 0)
	{
		list->setCurrentRow(0);
	}
	else
	{
		log->setPlainText("No crash logs found");
	}
}

void View::Crashes::logItemChanged(QListWidgetItem *current, QListWidgetItem */*previous*/)
{
	if (current == nullptr)
	{
		return;
	}

	auto role = static_cast<int>(CrashRole::StackTrace);
	log->setPlainText(current->data(role).toString());
}
