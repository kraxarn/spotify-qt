#pragma once

#include "widget/docktitle.hpp"

#include <QLayout>
#include <QGroupBox>

class WidgetUtils
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

	/** Find parent widget of specified type */
	template<typename T>
	static auto find(QWidget *from) -> T *
	{
		auto *w = from;
		while (w != nullptr && typeid(*w) != typeid(T))
		{
			w = w->parentWidget();
		}
		return dynamic_cast<T *>(w);
	}

private:
	WidgetUtils() = default;
};
