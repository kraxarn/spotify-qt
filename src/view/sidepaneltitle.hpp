#pragma once

#include <QTabBar>

namespace View
{
	class SidePanelTitle: public QTabBar
	{
	public:
		explicit SidePanelTitle(QWidget *parent);
	};
}
