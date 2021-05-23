#include "configview.hpp"

ConfigView::ConfigView(const lib::settings &settings, QWidget *parent)
	: settings(settings),
	QTreeWidget(parent)
{
	setHeaderLabels({
		"Key",
		"Value"
	});

	header()->setSectionsMovable(false);
	header()->setSectionResizeMode(QHeaderView::ResizeToContents);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested, this, &ConfigView::menu);
}

void ConfigView::menu(const QPoint &pos)
{
	auto *menu = new QMenu(this);
	auto *item = itemAt(pos);

	if (item != nullptr)
	{
		QAction::connect(menu->addAction("Copy key"),
			&QAction::triggered, [item](bool /*checked*/)
			{
				QApplication::clipboard()->setText(item->text(0));
			});

		QAction::connect(menu->addAction("Copy value"),
			&QAction::triggered, [item](bool /*checked*/)
			{
				QApplication::clipboard()->setText(item->text(1));
			});

		menu->addSeparator();
	}

	QAction::connect(menu->addAction(Icon::get("folder-txt"), "Open in external editor"),
		&QAction::triggered, [this](bool /*checked*/)
		{
			UrlUtils::open(this->settings.file_name(), LinkType::Path, this);
		});

	menu->popup(mapToGlobal(pos));
}

void ConfigView::reload()
{
	clear();

	const auto &json = settings.to_json();
	for (const auto &i : json.items())
	{
		auto *item = new QTreeWidgetItem(this);
		item->setText(0, QString::fromStdString(i.key()));

		for (const auto &ii : i.value().items())
		{
			auto *child = new QTreeWidgetItem(item);
			child->setText(0, QString::fromStdString(ii.key()));
			child->setText(1, QString::fromStdString(ii.value().dump()));
			child->setToolTip(1, ii.value().type_name());
		}
	}
}

void ConfigView::showEvent(QShowEvent * /*event*/)
{
	reload();
}
