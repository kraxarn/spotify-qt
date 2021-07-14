#include "view/sidepanel/title.hpp"
#include "view/sidepanel/sidepanel.hpp"

View::SidePanel::Title::Title(QWidget *parent)
	: QTabBar(parent)
{
	setMovable(true);
	setTabsClosable(true);
}

void View::SidePanel::Title::mouseReleaseEvent(QMouseEvent *event)
{
	auto *sidePanel = dynamic_cast<View::SidePanel::SidePanel *>(parentWidget());

	if (event->button() == Qt::MiddleButton
		&& sidePanel != nullptr)
	{
		sidePanel->removeTab(tabAt(event->localPos().toPoint()));
	}
}
