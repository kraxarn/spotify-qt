#include "util/menuaction.hpp"

auto MenuAction::create(const QString &iconName, const QString &text,
	QObject *parent, const QKeySequence &shortcut) -> QAction *
{
	auto *action = create(iconName, text, parent);
	action->setShortcut(shortcut);
	return action;
}

auto MenuAction::create(const QString &iconName, const QString &text,
	QObject *parent) -> QAction *
{
	return new QAction(Icon::get(iconName), text, parent);
}
