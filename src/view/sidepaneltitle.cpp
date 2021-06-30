#include "sidepaneltitle.hpp"

View::SidePanelTitle::SidePanelTitle(QWidget *parent)
	: QTabBar(parent)
{
	setMovable(true);
	setTabsClosable(true);
}
