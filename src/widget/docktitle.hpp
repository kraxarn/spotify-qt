#pragma once

#include <QLabel>
#include <QDockWidget>

namespace Widget
{
	class DockTitle: public QLabel
	{
	public:
		DockTitle(const QString &title, QDockWidget *parent);

	private:
		static constexpr int marginVertical = 4;
		static constexpr int marginHorizontal = 8;
	};
}
