#pragma once

#include <QTabBar>
#include <QMouseEvent>

namespace View
{
	class SidePanelTitle: public QTabBar
	{
	public:
		explicit SidePanelTitle(QWidget *parent);

	protected:
		void mouseReleaseEvent(QMouseEvent *event) override;
	};
}
