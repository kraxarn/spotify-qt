#include "util/menuaction.hpp"

auto MenuAction::create(const QString &iconName, const QString &text,
	QObject *parent, QKeySequence::StandardKey shortcut) -> QAction *
{
	auto *action = new QAction(Icon::get(iconName), text, parent);
	if (shortcut != QKeySequence::UnknownKey)
	{
		action->setShortcut(QKeySequence(shortcut));
	}
	return action;
}

auto MenuAction::create(const QString &iconName, const QString &text,
	QObject *parent) -> QAction *
{
	return create(iconName, text, parent, QKeySequence::UnknownKey);
}
