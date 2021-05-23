#pragma once

#include "util/icon.hpp"

#include <QString>
#include <QKeySequence>
#include <QAction>

class MenuUtils
{
public:
	/** Create a new action */
	static auto createAction(const QString &iconName,
		const QString &text, QObject *parent,
		QKeySequence::StandardKey shortcut = QKeySequence::UnknownKey) -> QAction *;

private:
	MenuUtils() = default;
};
