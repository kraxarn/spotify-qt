#include "configview.hpp"

ConfigView::ConfigView(const lib::Settings &settings, QWidget *parent)
	: QTreeWidget(parent)
{
	setHeaderLabels({
		"Key",
		"Value"
	});

	for (auto &i : settings.toJson().items())
	{
		auto item = new QTreeWidgetItem(this);
		item->setText(0, QString::fromStdString(std::string(i.key())));

		for (auto &ii : i.value().items())
		{
			auto child = new QTreeWidgetItem(item);
			child->setText(0, QString::fromStdString(std::string(ii.key())));
			child->setText(1, QString::fromStdString(std::string(ii.value())));
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
	QAction::connect(menu->addAction(Icon::get("folder-txt"), "Open in external editor"),
		&QAction::triggered,[this](bool)
	{
		Utils::openUrl(QString::fromStdString(lib::Settings::fileName()), LinkType::Path, this);
	});
	menu->popup(mapToGlobal(pos));
}