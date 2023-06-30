#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "util/tooltip.hpp"

#include <QListWidget>

namespace Search
{
	class Playlists: public QListWidget
	{
	Q_OBJECT

	public:
		Playlists(lib::spt::api &spotify, lib::cache &cache, const lib::http_client &httpClient,
			lib::settings &settings, QWidget *parent);

		void add(const lib::spt::playlist &playlist);

	private:
		lib::spt::api &spotify;
		lib::cache &cache;
		Tooltip tooltip;

		void onItemClicked(QListWidgetItem *item);
		void onItemDoubleClicked(QListWidgetItem *item);
		void onContextMenu(const QPoint &pos);
		void onItemEntered(QListWidgetItem *item);
	};
}
