#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include <QTreeWidget>
#include <QHeaderView>

namespace View
{
	namespace Artist
	{
		class AlbumList: public QTreeWidget
		{
		public:
			AlbumList(lib::spt::api &spotify, lib::cache &cache, QWidget *parent);

		private:
			lib::spt::api &spotify;
			lib::cache &cache;

			static auto albumId(QTreeWidgetItem *item) -> std::string;

			void onItemClicked(QTreeWidgetItem *item, int column);
			void onItemDoubleClicked(QTreeWidgetItem *item, int column);
			void onContextMenu(const QPoint &pos);
		};
	}
}
