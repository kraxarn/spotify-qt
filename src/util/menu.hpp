#pragma once

#include "util/icon.hpp"

#include <QString>
#include <QKeySequence>
#include <QAction>

class Menu
{
public:
	/**
	 * Create a new action with specified shortcut
	 */
	static auto createAction(const QString &iconName,
		const QString &text, QObject *parent,
		QKeySequence::StandardKey shortcut) -> QAction *;

	/**
	 * Create a new action without a shortcut
	 */
	static auto createAction(const QString &iconName,
		const QString &text, QObject *parent) -> QAction *;

private:
	Menu() = default;
};
