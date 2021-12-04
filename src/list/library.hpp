#pragma once

#include "lib/spotify/api.hpp"

#include "util/treeutils.hpp"
#include "listitem/library.hpp"

#include <QTreeWidget>
#include <QHeaderView>

namespace List
{
	class Library: public QTreeWidget
	{
	Q_OBJECT

	public:
		Library(lib::spt::api &spotify, QWidget *parent);

	private:
		lib::spt::api &spotify;

		static constexpr const char *followedArtists = "Followed Artists";
		static constexpr const char *newReleases = "New Releases";
		static constexpr const char *recentlyPlayed = "History";
		static constexpr const char *savedAlbums = "Liked Albums";
		static constexpr const char *savedTracks = "Liked Tracks";
		static constexpr const char *topArtists = "Top Artists";
		static constexpr const char *topTracks = "Top Tracks";

		void clicked(QTreeWidgetItem *item, int column);
		void doubleClicked(QTreeWidgetItem *item, int column);
		void expanded(QTreeWidgetItem *item);

		void tracksLoaded(const std::string &id, const std::vector<lib::spt::track> &tracks);
		static void itemsLoaded(std::vector<ListItem::Library> &items, QTreeWidgetItem *item);
	};
}
