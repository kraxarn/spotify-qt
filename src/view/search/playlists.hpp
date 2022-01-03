#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include <QListWidget>

namespace Search
{
	class Playlists: public QListWidget
	{
	Q_OBJECT

	public:
		Playlists(lib::spt::api &spotify, lib::cache &cache, QWidget *parent);

		void add(const lib::spt::playlist &playlist);

	private:
		lib::spt::api &spotify;
		lib::cache &cache;

		void onItemClicked(QListWidgetItem *item);
		void onItemDoubleClicked(QListWidgetItem *item);
		void onContextMenu(const QPoint &pos);
	};
}
