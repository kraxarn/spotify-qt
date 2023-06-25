#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "lib/enum/albumgroup.hpp"

#include <QTreeWidget>
#include <QHeaderView>

#include <map>

namespace Artist
{
	class AlbumsList: public QTreeWidget
	{
	public:
		AlbumsList(lib::spt::api &spotify, lib::cache &cache,
			const lib::http_client &httpClient, QWidget *parent);

		void setAlbums(const std::vector<lib::spt::album> &albums);

	private:
		lib::spt::api &spotify;
		lib::cache &cache;
		const lib::http_client &httpClient;

		std::map<lib::album_group, QTreeWidgetItem *> groups;

		static auto getAlbum(QTreeWidgetItem *item) -> lib::spt::album;
		static auto groupToString(lib::album_group albumGroup) -> QString;

		void onItemClicked(QTreeWidgetItem *item, int column);
		void onItemDoubleClicked(QTreeWidgetItem *item, int column);
		void onContextMenu(const QPoint &pos);
	};
}
