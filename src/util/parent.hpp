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
		auto *widget = from;
		while (widget != nullptr && typeid(*widget) != typeid(T))
		{
			widget = widget->parentWidget();
		}
		return qobject_cast<T *>(widget);
	}

	/**
	 * Find parent object  of specified type
	 */
	template<typename T>
	static auto findObject(QObject *from) -> T *
	{
		auto *object = from;
		while (object != nullptr && typeid(*object) != typeid(T))
		{
			object = object->parent();
		}
		return qobject_cast<T *>(object);
	}

private:
	Parent() = default;
};
