#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "lib/enum/albumgroup.hpp"

#include <QTreeWidget>
#include <QHeaderView>

#include <map>

namespace View
{
	namespace Artist
	{
		class AlbumList: public QTreeWidget
		{
		public:
			AlbumList(lib::spt::api &spotify, lib::cache &cache,
				const lib::http_client &httpClient, QWidget *parent);

			void setAlbums(const std::vector<lib::spt::album> &albums);

		private:
			lib::spt::api &spotify;
			lib::cache &cache;
			const lib::http_client &httpClient;

			std::map<lib::album_group, QTreeWidgetItem *> groups;

			static auto albumId(QTreeWidgetItem *item) -> std::string;
			static auto groupToString(lib::album_group albumGroup) -> QString;

			void onItemClicked(QTreeWidgetItem *item, int column);
			void onItemDoubleClicked(QTreeWidgetItem *item, int column);
			void onContextMenu(const QPoint &pos);
		};
	}
}
