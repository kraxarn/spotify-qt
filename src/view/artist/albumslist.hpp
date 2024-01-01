#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "lib/enum/albumgroup.hpp"
#include "util/tooltip.hpp"

#include <QTreeWidget>
#include <QHeaderView>

#include <map>

namespace Artist
{
	class AlbumsList: public QTreeWidget
	{
	public:
		AlbumsList(lib::spt::api &spotify, lib::cache &cache,
			const lib::http_client &httpClient, lib::settings &settings, QWidget *parent);

		void loadAlbums(const lib::spt::page<lib::spt::album> &page);

	private:
		lib::spt::api &spotify;
		lib::cache &cache;
		const lib::http_client &httpClient;
		Tooltip tooltip;

		std::map<lib::album_group, QTreeWidgetItem *> groups;

		static auto getAlbum(QTreeWidgetItem *item) -> lib::spt::album;
		static auto groupToString(lib::album_group albumGroup) -> QString;

		void onItemClicked(QTreeWidgetItem *item, int column);
		void onItemDoubleClicked(QTreeWidgetItem *item, int column);
		void onContextMenu(const QPoint &pos);
		void onItemEntered(QTreeWidgetItem *item, int column);
	};
}
