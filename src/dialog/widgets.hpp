#pragma once

#include "dialog/base.hpp"

#include <QTreeWidget>

namespace Dialog
{
	class Widgets: public Base
	{
	public:
		explicit Widgets(QWidget *parent);

	protected:
		void showEvent(QShowEvent *event) override;

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
