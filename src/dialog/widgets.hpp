#pragma once

#include "dialog/base.hpp"

#include <QTreeWidget>

namespace Dialog
{
	class Widgets: public Base
	{
	public:
		explicit Widgets(QWidget *parent);

	private:
		QTabWidget *tabs;

		auto tree(const QStringList &labels) -> QTreeWidget *;
		void refresh(int index);

		auto activeWindow() -> QWidget *;
		auto topLevelWindows() -> QWidget *;
		auto topLevelWidgets() -> QWidget *;

		void onRefreshClicked(bool checked);
	};
}
