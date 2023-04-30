#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "util/tree.hpp"
#include "listitem/library.hpp"

#include <QTreeWidget>
#include <QHeaderView>

namespace List
{
	class Library: public QTreeWidget
	{
	Q_OBJECT

	public:
		Library(lib::spt::api &spotify, lib::cache &cache, QWidget *parent);

		void load(QTreeWidgetItem *item);

	private:
		lib::spt::api &spotify;
		lib::cache &cache;

		static constexpr const int dataRole = 0x100;
		static constexpr const char *followedArtists = "Followed Artists";
		static constexpr const char *newReleases = "New Releases";
		static constexpr const char *recentlyPlayed = "History";
		static constexpr const char *savedAlbums = "Liked Albums";
		static constexpr const char *savedTracks = "Liked Tracks";
		static constexpr const char *topArtists = "Top Artists";
		static constexpr const char *topTracks = "Top Tracks";

		void onClicked(QTreeWidgetItem *item, int column);
		void onDoubleClicked(QTreeWidgetItem *item, int column);
		void onExpanded(QTreeWidgetItem *item);
		void onMenuRequested(const QPoint &pos);

		void tracksLoaded(const lib::spt::entity &entity, const std::vector<lib::spt::track> &tracks);
		static void itemsLoaded(std::vector<ListItem::Library> &items, QTreeWidgetItem *item);
	};
}
