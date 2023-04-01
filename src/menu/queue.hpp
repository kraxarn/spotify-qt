#pragma once

#include "lib/spotify/api.hpp"
#include "menu/base.hpp"

namespace Menu
{
	class Queue: public Base
	{
	Q_OBJECT

	public:
		Queue(lib::spt::api &spotify, QWidget *parent);

	private:
		lib::spt::api &spotify;

		void refreshQueue();
		void skipTracks(int skips);

		void onTriggered(QAction *action);
	};
}
