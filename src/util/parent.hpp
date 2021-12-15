#pragma once

#include <QWidget>

class Parent
{
public:
	/**
	 * Find parent widget of specified type
	 */
	template<typename T>
	static auto findWidget(QWidget *from) -> T *
	{
		auto *w = from;
		while (w != nullptr && typeid(*w) != typeid(T))
		{
			w = w->parentWidget();
		}
		return qobject_cast<T *>(w);
	}

	/**
	 * Find parent object  of specified type
	 */
	template<typename T>
	static auto findObject(QObject *from) -> T *
	{
		auto *w = from;
		while (w != nullptr && typeid(*w) != typeid(T))
		{
			w = w->parent();
		}
		return qobject_cast<T *>(w);
	}

private:
	Parent() = default;
};
