#include "menu/base.hpp"

Menu::Base::Base(QWidget *parent)
	: QMenu(parent)
{
}

void Menu::Base::addMessage(const QString &message)
{
	auto *action = addAction(message);
	action->setDisabled(true);
}
