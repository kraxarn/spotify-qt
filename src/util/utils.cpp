#include "utils.hpp"

#include "icon.hpp"
#include "../dialog/openlinkdialog.hpp"

bool Utils::darkBackground = false;

void Utils::applyPalette(lib::palette palette)
{
	QPalette p;
	switch (palette)
	{
		case lib::palette_app:
			p = QApplication::palette();
			break;

		case lib::palette_style:
			p = QApplication::style()->standardPalette();
			break;

		case lib::palette_dark:
			p = DarkPalette();
			break;
	}

	QApplication::setPalette(p);
}

auto Utils::layoutToWidget(QLayout *layout) -> QWidget *
{
	auto *widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}

auto Utils::createGroupBox(QVector<QWidget *> &widgets, QWidget *parent) -> QGroupBox *
{
	auto *group = new QGroupBox(parent);
	auto *layout = new QVBoxLayout();
	for (auto &widget : widgets)
	{
		layout->addWidget(widget);
	}
	group->setLayout(layout);
	return group;
}

auto Utils::createMenuAction(const QString &iconName, const QString &text,
	QObject *parent, QKeySequence::StandardKey shortcut) -> QAction *
{
	auto *action = new QAction(Icon::get(iconName), text, parent);
	if (shortcut != QKeySequence::UnknownKey)
	{
		action->setShortcut(QKeySequence(shortcut));
	}
	return action;
}
