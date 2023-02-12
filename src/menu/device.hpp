#pragma once

#include "lib/spotify/api.hpp"
#include <QMenu>

namespace Menu
{
	class Device: public QMenu
	{
	Q_OBJECT

	public:
		Device(lib::spt::api &spotify, QWidget *parent);

	private:
		lib::spt::api &spotify;

		void refreshDevices();

		void onAboutToShow();
		void onTriggered(QAction *action);
	};
}
