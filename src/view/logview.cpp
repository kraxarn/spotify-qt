#include "logview.hpp"

LogView::LogView(QWidget *parent)
	: QWidget(parent)
{
	auto layout = new QVBoxLayout();
	setLayout(layout);

	auto list = new QTreeWidget(this);
	list->setHeaderLabels({
		"Time", "Type", "Message"
	});
	list->setEditTriggers(QAbstractItemView::NoEditTriggers);
	list->setSelectionBehavior(QAbstractItemView::SelectRows);
	list->setRootIsDecorated(false);
	list->setAllColumnsShowFocus(true);
	layout->addWidget(list, 1);

	auto buttons = new QHBoxLayout();
	buttons->setAlignment(Qt::AlignRight);

	auto copyToClipboard = new QPushButton("Copy to clipboard", this);
	buttons->addWidget(copyToClipboard);
	QPushButton::connect(copyToClipboard, &QPushButton::clicked, this, &LogView::copyToClipboard);

	auto save = new QPushButton("Save...", this);
	buttons->addWidget(save);
	QPushButton::connect(save, &QPushButton::clicked, this, &LogView::saveToFile);

	layout->addLayout(buttons);

	for (auto &message : Log::getMessages())
	{
		auto item = new QTreeWidgetItem({
			message.getTime(), message.getType(), message.getMessage()
		});

		list->addTopLevelItem(item);
	}
}

QString LogView::collectLogs()
{
	QStringList items;
	for (auto &message : Log::getMessages())
	{
		items.append(QString::fromStdString(message.format()));
	}
	return items.join('\n');
}

void LogView::saveToFile()
{
	auto fileName = QFileDialog::getSaveFileName(this,
		"Select location",
		QString("%1/spotify-qt-%2.log")
			.arg(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first())
			.arg(QDateTime::currentDateTime().toString("yyyyMMdd")),
		"Logs (*.log)");

	if (fileName.isEmpty())
		return;

	QFile out(fileName);
	out.open(QIODevice::WriteOnly);
	out.write(collectLogs().toUtf8());
	out.close();
}

void LogView::copyToClipboard()
{
	QApplication::clipboard()->setText(collectLogs());
}
