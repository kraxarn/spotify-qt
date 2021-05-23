#include "utils.hpp"

#include "icon.hpp"
#include "../dialog/openlinkdialog.hpp"

bool Utils::darkBackground = false;

void Utils::applyPalette(lib::palette palette)
{
	QPalette p;
	switch (palette)
	{
		case lib::palette_app:
			p = QApplication::palette();
			break;

		case lib::palette_style:
			p = QApplication::style()->standardPalette();
			break;

		case lib::palette_dark:
			p = DarkPalette();
			break;
	}

	QApplication::setPalette(p);
}

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
