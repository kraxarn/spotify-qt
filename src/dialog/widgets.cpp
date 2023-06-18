#include "dialog/widgets.hpp"

#include <QPushButton>

Dialog::Widgets::Widgets(QWidget *parent)
	: Base(parent)
{
	setWindowTitle(QStringLiteral("Widgets"));
	auto *layout = Base::layout<QVBoxLayout>();

	tabs = new QTabWidget(this);
	tabs->addTab(activeWindow(), QStringLiteral("Active window"));
	tabs->addTab(topLevelWindows(), QStringLiteral("Top-level windows"));
	tabs->addTab(topLevelWidgets(), QStringLiteral("Top-level widgets"));
	layout->addWidget(tabs);

	auto *refreshButton = Base::addButton(QStringLiteral("Refresh"), QDialogButtonBox::ResetRole);
	QPushButton::connect(refreshButton, &QPushButton::clicked,
		this, &Dialog::Widgets::onRefreshClicked);

	Base::addAction(DialogAction::Ok);
}

void Dialog::Widgets::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);

	for (auto i = 0; i < tabs->children().length(); i++)
	{
		refresh(i);
	}
}

auto Dialog::Widgets::tree(const QStringList &labels) -> QTreeWidget *
{
	auto *tree = new QTreeWidget(this);
	tree->setHeaderLabels(labels);
	tree->setRootIsDecorated(false);
	return tree;
}

auto Dialog::Widgets::activeWindow() -> QWidget *
{
	return tree({
		QStringLiteral("Class name"),
	});
}

auto Dialog::Widgets::topLevelWindows() -> QWidget *
{
	return tree({
		QStringLiteral("Class name"),
	});
}

auto Dialog::Widgets::topLevelWidgets() -> QWidget *
{
	return tree({
		QStringLiteral("Class name"),
		QStringLiteral("Parent class name"),
	});
}

void Dialog::Widgets::refresh(int index)
{
	auto *child = tabs->widget(index);
	auto *tree = qobject_cast<QTreeWidget *>(child);
	if (tree == nullptr)
	{
		return;
	}

	tree->clear();
	if (index == 0)
	{
		// Active window
		auto *item = new QTreeWidgetItem(tree);
		const auto *window = QApplication::activeWindow();
		item->setText(0, window->metaObject()->className());
	}
	else if (index == 1)
	{
		// Top-level windows
		for (const auto *window: QApplication::topLevelWindows())
		{
			auto *item = new QTreeWidgetItem(tree);
			item->setText(0, window->metaObject()->className());
		}
	}
	else if (index == 2)
	{
		// Top-level widgets
		for (const auto *widget: QApplication::topLevelWidgets())
		{
			auto *item = new QTreeWidgetItem(tree);
			item->setText(0, widget->metaObject()->className());

			auto *parent = widget->parent();
			if (parent != nullptr)
			{
				item->setText(1, widget->parent()->metaObject()->className());
			}
		}
	}
}

void Dialog::Widgets::onRefreshClicked(bool /*checked*/)
{
	refresh(tabs->currentIndex());
}
