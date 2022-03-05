#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include <QMenu>

namespace Menu
{
	class AddToPlaylist: public QMenu
	{
	public:
		AddToPlaylist(std::vector<std::string> trackIds, lib::spt::api &spotify,
			const lib::cache &cache, QWidget *parent);

	private:
		std::vector<std::string> trackIds;
		lib::spt::api &spotify;
		const lib::cache &cache;

		void onAboutToShow();
		void onTriggered(QAction *action);

		auto getFilteredTrackIds(const std::vector<lib::spt::track> &tracks)
		-> std::vector<std::string>;

		void addToNewPlaylist();
		void addToPlaylist(const lib::spt::playlist &playlist);
	};
}
