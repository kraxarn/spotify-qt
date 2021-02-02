#include "configview.hpp"

ConfigView::ConfigView(const lib::settings &settings, QWidget *parent)
	: settings(settings),
	QWidget(parent)
{
	auto layout = new QVBoxLayout(this);

	onlyNonDefault = new QCheckBox("Only show non-default settings", this);
	onlyNonDefault->setToolTip("Only show settings manually changed by the user, "
							   "can be used to troubleshoot issues");
	onlyNonDefault->setChecked(false);
	layout->addWidget(onlyNonDefault);
	QCheckBox::connect(onlyNonDefault, &QCheckBox::stateChanged, [this](int)
	{
		this->reload();
	});

	tree = new QTreeWidget(this);
	layout->addWidget(tree, 1);

	tree->setHeaderLabels({
		"Key",
		"Value"
	});

	tree->header()->setSectionsMovable(false);
	tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

	tree->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(tree, &QWidget::customContextMenuRequested, this, &ConfigView::menu);
}

void ConfigView::menu(const QPoint &pos)
{
	auto menu = new QMenu(this);
	auto item = tree->itemAt(pos);

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
			Utils::openUrl(QString::fromStdString(this->settings.file_name()),
				LinkType::Path, this);
		});

	menu->popup(mapToGlobal(pos));
}

void ConfigView::reload()
{
	tree->clear();

	auto json = settings.to_json();
	for (auto &i : json.items())
	{
		auto item = new QTreeWidgetItem(tree);
		item->setText(0, QString::fromStdString(i.key()));

		for (auto &ii : i.value().items())
		{
			auto child = new QTreeWidgetItem(item);
			child->setText(0, QString::fromStdString(ii.key()));
			child->setText(1, QString::fromStdString(ii.value().dump()));
		}
	}
}

void ConfigView::showEvent(QShowEvent*)
{
	reload();
}
