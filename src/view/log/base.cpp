#include "view/log/base.hpp"
#include "metatypes.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QClipboard>
#include <QStandardPaths>
#include <QDateTime>
#include <QFileDialog>

Log::Base::Base(QWidget *parent)
	: QWidget(parent)
{
	auto *layout = new QVBoxLayout(this);

	list = new QTreeWidget(this);
	list->setHeaderLabels({
		QStringLiteral("Time"),
		QStringLiteral("Type"),
		QStringLiteral("Message"),
	});
	list->setEditTriggers(QAbstractItemView::NoEditTriggers);
	list->setSelectionBehavior(QAbstractItemView::SelectRows);
	list->setRootIsDecorated(false);
	list->setAllColumnsShowFocus(true);
	layout->addWidget(list, 1);

	auto *buttons = new QHBoxLayout();
	buttons->setAlignment(Qt::AlignRight);
	layout->addLayout(buttons);

	auto *copyToClipboard = new QPushButton(QStringLiteral("Copy to clipboard"), this);
	buttons->addWidget(copyToClipboard);

	QPushButton::connect(copyToClipboard, &QPushButton::clicked,
		this, &Log::Base::onCopyToClipboard);

	auto *save = new QPushButton(QStringLiteral("Save..."), this);
	buttons->addWidget(save);

	QPushButton::connect(save, &QPushButton::clicked,
		this, &Log::Base::onSaveToFile);
}

void Log::Base::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);

	list->clear();
	for (const auto &message: getMessages())
	{
		auto *item = new QTreeWidgetItem({
			QString::fromStdString(message.get_time()),
			QString::fromStdString(message.get_type()),
			QString::fromStdString(message.get_message()),
		});

		item->setData(0, messageRole, QVariant::fromValue(message));
		list->addTopLevelItem(item);
	}
}

auto Log::Base::collectLogs() -> QString
{
	QStringList items;

	for (auto i = 0; i < list->topLevelItemCount(); i++)
	{
		auto *item = list->topLevelItem(i);
		const auto &data = item->data(0, messageRole);
		const auto &message = data.value<lib::log_message>();

		items.append(QString::fromStdString(message.to_string()));
	}

	return items.join('\n');
}

void Log::Base::onCopyToClipboard(bool /*checked*/)
{
	QApplication::clipboard()->setText(collectLogs());
}

void Log::Base::onSaveToFile(bool /*checked*/)
{
	const auto location = QStandardPaths::DocumentsLocation;
	const auto path = QStandardPaths::standardLocations(location).first();
	const auto date = QDateTime::currentDateTime().toString("yyyyMMdd");

	const auto filename = QFileDialog::getSaveFileName(this,
		QStringLiteral("Select location"),
		QString("%1/spotify-qt-%2.log").arg(path, date),
		QStringLiteral("Logs (*.log)"));

	if (filename.isEmpty())
	{
		return;
	}

	QFile out(filename);
	out.open(QIODevice::WriteOnly);
	out.write(collectLogs().toUtf8());
	out.close();
}
