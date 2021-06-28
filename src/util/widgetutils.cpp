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

auto WidgetUtils::createDockWidget(QWidget *widget, QWidget *parent) -> QDockWidget *
{
	auto *dock = new QDockWidget(parent);
	dock->setWidget(widget);
	return dock;
}

auto WidgetUtils::createDockWidget(QWidget *widget, const QString &title,
	QWidget *parent) -> QDockWidget *
{
	auto *dock = createDockWidget(widget, parent);
	dock->setTitleBarWidget(new Widget::DockTitle(title, dock));
	dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
	return dock;
}

auto WidgetUtils::createDockWidget(QWidget *widget, const QString &title,
	const QMargins &margins, QWidget *parent) -> QDockWidget *
{
	auto *dock = createDockWidget(widget, title, parent);
	dock->setContentsMargins(margins);
	return dock;
}
