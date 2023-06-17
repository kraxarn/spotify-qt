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
		auto tree(const QStringList &labels);

		auto activeWindow() -> QWidget *;
		auto topLevelWindows() -> QWidget *;
		auto topLevelWidgets() -> QWidget *;
	};
}
