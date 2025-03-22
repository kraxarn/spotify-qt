#include "view/log/base.hpp"
#include "metatypes.hpp"
#include "util/icon.hpp"

#include <QVBoxLayout>
#include <QPushButton>
#include <QApplication>
#include <QClipboard>
#include <QStandardPaths>
#include <QDateTime>
#include <QFileDialog>
#include <QMenu>

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

	list->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(list, &QWidget::customContextMenuRequested,
		this, &Log::Base::onMenuRequested);
}

void Log::Base::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);

	list->clear();
	for (const auto &logMessage: getMessages())
	{
		const auto time = QString::fromStdString(logMessage.get_time());
		const auto type = QString::fromStdString(logMessage.get_type());
		const auto message = QString::fromStdString(logMessage.get_message());

		auto *item = new QTreeWidgetItem({
			time,
			type,
			message,
		});

		item->setToolTip(0, time);
		item->setToolTip(1, type);
		item->setToolTip(2, message);

		item->setData(0, messageRole, QVariant::fromValue(logMessage));
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
		QStringLiteral("%1/spotify-qt-%2.log").arg(path, date),
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

void Log::Base::onMenuRequested(const QPoint &pos)
{
	auto *item = list->itemAt(pos);
	if (item == nullptr)
	{
		return;
	}

	auto *menu = new QMenu(this);

	auto *copyToClipboard = menu->addAction(Icon::get(QStringLiteral("edit-copy")),
		QStringLiteral("Copy to clipboard"));

	QAction::connect(copyToClipboard, &QAction::triggered, [item](bool /*checked*/)
	{
		const auto &data = item->data(0, messageRole);
		const auto &message = data.value<lib::log_message>();

		QApplication::clipboard()->setText(QString::fromStdString(message.to_string()));
	});

	menu->popup(list->mapToGlobal(pos));
}
