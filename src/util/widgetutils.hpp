#pragma once

#include "widget/docktitle.hpp"

#include <QLayout>
#include <QGroupBox>
#include <QDockWidget>

class WidgetUtils
{
public:
	/** Wrap layout in widget */
	static auto layoutToWidget(QLayout *layout, QWidget *parent) -> QWidget *;

	/** Create a new group box with specified widgets in a vertical layout */
	static auto createGroupBox(QVector<QWidget *> &widgets,
		QWidget *parent) -> QGroupBox *;

	/** Create a new group box with specified widget in a vertical layout */
	static auto createGroupBox(QWidget *widget, QWidget *parent) -> QGroupBox *;

	/** Wrap widget in dock widget */
	static auto createDockWidget(QWidget *widget, QWidget *parent) -> QDockWidget *;

	/** Wrap widget in dock widget with specified title */
	static auto createDockWidget(QWidget *widget, const QString &title,
		QWidget *parent) -> QDockWidget *;

	/** Wrap widget in dock widget with specified title and margins */
	static auto createDockWidget(QWidget *widget, const QString &title,
		const QMargins &margins, QWidget *parent) -> QDockWidget *;

private:
	WidgetUtils() = default;
};
