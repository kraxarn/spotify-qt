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
	for (const auto &crash : cache.get_all_crashes())
	{
		auto dateTime = QDateTime::fromSecsSinceEpoch(crash.timestamp);
		auto dateString = QLocale::system().toString(dateTime, QLocale::ShortFormat);

		auto *item = new QListWidgetItem(dateString);
		item->setData(stack_trace_role, QString::fromStdString(crash.to_string()));
		list->addItem(item);
	}
}

void View::Crashes::logItemChanged(QListWidgetItem *current, QListWidgetItem */*previous*/)
{
	log->setPlainText(current->data(stack_trace_role).toString());
}
