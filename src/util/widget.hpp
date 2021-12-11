#pragma once

#include "widget/docktitle.hpp"

#include <QLayout>
#include <QGroupBox>

class Widget
{
public:
	/** Wrap layout in widget */
	static auto layoutToWidget(QLayout *layout, QWidget *parent) -> QWidget *;

	/** Wrap widget in dock widget */
	static auto createDockWidget(QWidget *widget, QWidget *parent) -> QDockWidget *;

	/** Wrap widget in dock widget with specified title */
	static auto createDockWidget(QWidget *widget, const QString &title,
		QWidget *parent) -> QDockWidget *;

	/** Wrap widget in dock widget with specified title and margins */
	static auto createDockWidget(QWidget *widget, const QString &title,
		const QMargins &margins, QWidget *parent) -> QDockWidget *;

private:
	Widget() = default;
};
