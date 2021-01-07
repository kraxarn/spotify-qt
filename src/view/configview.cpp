#include "configview.hpp"

ConfigView::ConfigView(const lib::settings &settings, QWidget *parent)
	: settings(settings),
	QTreeWidget(parent)
{
	setHeaderLabels({
		"Key",
		"Value"
	});

	auto json = settings.toJson();
	for (auto &key : json.keys())
	{
		auto item = new QTreeWidgetItem(this);
		item->setText(0, key);

		auto obj = json[key].toObject();
		for (auto &subKey : obj.keys())
		{
			auto child = new QTreeWidgetItem(item);
			child->setText(0, subKey);
			child->setText(1, obj[subKey].toVariant().toString());
		}
	}

	header()->setSectionsMovable(false);
	header()->setSectionResizeMode(QHeaderView::ResizeToContents);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested, this, &ConfigView::menu);
}

void ConfigView::menu(const QPoint &pos)
{
	auto menu = new QMenu(this);
	auto item = itemAt(pos);

	if (item != nullptr)
	{
		QAction::connect(menu->addAction("Copy key"), &QAction::triggered, [item](bool)
		{
			QApplication::clipboard()->setText(item->text(0));
		});

		QAction::connect(menu->addAction("Copy value"), &QAction::triggered, [item](bool)
		{
			QApplication::clipboard()->setText(item->text(1));
		});

		menu->addSeparator();
	}

	QAction::connect(menu->addAction(Icon::get("folder-txt"), "Open in external editor"),
		&QAction::triggered, [this](bool)
		{
			Utils::openUrl(this->settings.fileName(), LinkType::Path, this);
		});

	menu->popup(mapToGlobal(pos));
}