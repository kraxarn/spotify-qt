#pragma once

#include <QTabBar>
#include <QMouseEvent>

namespace View
{
	namespace SidePanel
	{
		class Title: public QTabBar
		{
		public:
			explicit Title(QWidget *parent);

		protected:
			void mouseReleaseEvent(QMouseEvent *event) override;
		};
	}
}
