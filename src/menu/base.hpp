#pragma once

#include <QMenu>

namespace Menu
{
	class Base: public QMenu
	{
	protected:
		explicit Base(QWidget *parent);

		void addMessage(const QString &message);
	};
}
