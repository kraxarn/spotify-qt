#include "sidepaneltitle.hpp"
#include "view/sidepanel.hpp"

View::SidePanelTitle::SidePanelTitle(QWidget *parent)
	: QTabBar(parent)
{
	setMovable(true);
	setTabsClosable(true);
}

void View::SidePanelTitle::mouseReleaseEvent(QMouseEvent *event)
{
	auto *sidePanel = dynamic_cast<SidePanel *>(parentWidget());

	if (event->button() == Qt::MiddleButton
		&& sidePanel != nullptr)
	{
		sidePanel->removeTab(tabAt(event->localPos().toPoint()));
	}
}
