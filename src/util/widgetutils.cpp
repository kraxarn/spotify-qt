#include "widgetutils.hpp"

auto WidgetUtils::layoutToWidget(QLayout *layout, QWidget *parent) -> QWidget *
{
	auto *widget = new QWidget(parent);
	widget->setLayout(layout);
	return widget;
}

auto WidgetUtils::createGroupBox(QVector<QWidget *> &widgets, QWidget *parent) -> QGroupBox *
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

auto WidgetUtils::createGroupBox(QWidget *widget, QWidget *parent) -> QGroupBox *
{
	QVector<QWidget *> widgets{
		widget
	};
	return createGroupBox(widgets, parent);
}

auto WidgetUtils::createDockWidget(QWidget *widget, QWidget *parent) -> QDockWidget *
{
	auto *dock = new QDockWidget(parent);
	dock->setWidget(widget);
	return dock;
}
