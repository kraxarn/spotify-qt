#pragma once

#include "lib/spotify/api.hpp"
#include <QMenu>

namespace Menu
{
	class Queue: public QMenu
	{
	Q_OBJECT

	public:
		Queue(lib::spt::api &spotify, QWidget *parent);

	private:
		lib::spt::api &spotify;

		void addMessage(const QString &message);
		void refreshQueue();
		void skipTracks(int skips);

		void onTriggered(QAction *action);
	};
}
