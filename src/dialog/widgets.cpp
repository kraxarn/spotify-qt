#include "dialog/widgets.hpp"

Dialog::Widgets::Widgets(QWidget *parent)
	: Base(parent)
{
	setWindowTitle(QStringLiteral("Widgets"));
	auto *layout = Base::layout<QVBoxLayout>();

	auto *tabs = new QTabWidget(this);
	tabs->addTab(activeWindow(), QStringLiteral("Active window"));
	tabs->addTab(topLevelWindows(), QStringLiteral("Top-level windows"));
	tabs->addTab(topLevelWidgets(), QStringLiteral("Top-level widgets"));
	layout->addWidget(tabs);

	Base::addAction(DialogAction::Ok);
}

auto Dialog::Widgets::tree(const QStringList &labels)
{
	auto *tree = new QTreeWidget(this);
	tree->setHeaderLabels(labels);
	tree->setRootIsDecorated(false);
	return tree;
}

auto Dialog::Widgets::activeWindow() -> QWidget *
{
	auto *items = tree({
		QStringLiteral("Class name"),
	});

	auto *item = new QTreeWidgetItem(items);
	const auto *window = QApplication::activeWindow();
	item->setText(0, window->metaObject()->className());

	return items;
}

auto Dialog::Widgets::topLevelWindows() -> QWidget *
{
	auto *items = tree({
		QStringLiteral("Class name"),
	});

	for (const auto *window: QApplication::topLevelWindows())
	{
		auto *item = new QTreeWidgetItem(items);
		item->setText(0, window->metaObject()->className());
	}

	return items;
}

auto Dialog::Widgets::topLevelWidgets() -> QWidget *
{
	auto *items = tree({
		QStringLiteral("Class name"),
		QStringLiteral("Parent class name"),
	});

	for (const auto *widget: QApplication::topLevelWidgets())
	{
		auto *item = new QTreeWidgetItem(items);
		item->setText(0, widget->metaObject()->className());

		auto *parent = widget->parent();
		if (parent != nullptr)
		{
			item->setText(1, widget->parent()->metaObject()->className());
		}
	}

	return items;
}
