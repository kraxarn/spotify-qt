#pragma once

#include "util/icon.hpp"
#include "enum/shortcut.hpp"

#include <QString>
#include <QKeySequence>
#include <QAction>

class MenuAction
{
public:
	/**
	 * Create a new action with specified standard shortcut
	 */
	static auto create(const QString &iconName, const QString &text, QObject *parent,
		QKeySequence::StandardKey shortcut) -> QAction *;

	/**
	 * Create a new action with specified application shortcut
	 */
	static auto create(const QString &iconName, const QString &text, QObject *parent,
		Shortcut shortcut) -> QAction *;

	/**
	 * Create a new action without a shortcut
	 */
	static auto create(const QString &iconName,
		const QString &text, QObject *parent) -> QAction *;

private:
	MenuAction() = default;
};
