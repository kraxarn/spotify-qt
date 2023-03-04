#include "view/sidepanel/title.hpp"
#include "view/sidepanel/view.hpp"

SidePanel::Title::Title(QWidget *parent)
	: QTabBar(parent)
{
	setMovable(true);
	setTabsClosable(true);
	setUsesScrollButtons(true);
}

void SidePanel::Title::mouseReleaseEvent(QMouseEvent *event)
{
	auto *sidePanel = dynamic_cast<SidePanel::View *>(parentWidget());

	if (event->button() == Qt::MiddleButton
		&& sidePanel != nullptr)
	{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		const auto pos = event->localPos();
#else
		const auto pos = event->position();
#endif
		sidePanel->removeTab(tabAt(pos.toPoint()));
	}

	QTabBar::mouseReleaseEvent(event);
}
