#include "view/sidepanel/title.hpp"
#include "view/sidepanel/view.hpp"

SidePanel::Title::Title(QWidget *parent)
	: QTabBar(parent)
{
	setMovable(true);
	setTabsClosable(true);
}

void SidePanel::Title::mouseReleaseEvent(QMouseEvent *event)
{
	auto *sidePanel = dynamic_cast<SidePanel::View *>(parentWidget());

	if (event->button() == Qt::MiddleButton
		&& sidePanel != nullptr)
	{
		sidePanel->removeTab(tabAt(event->localPos().toPoint()));
	}
}
