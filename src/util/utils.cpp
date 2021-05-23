#include "utils.hpp"

#include "icon.hpp"
#include "../dialog/openlinkdialog.hpp"

auto Utils::createMenuAction(const QString &iconName, const QString &text,
	QObject *parent, QKeySequence::StandardKey shortcut) -> QAction *
{
	auto *action = new QAction(Icon::get(iconName), text, parent);
	if (shortcut != QKeySequence::UnknownKey)
	{
		action->setShortcut(QKeySequence(shortcut));
	}
	return action;
}
