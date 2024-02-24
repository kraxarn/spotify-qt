#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include <QMenu>

namespace Menu
{
	class AddToPlaylist: public QMenu
	{
	Q_OBJECT

	public:
		AddToPlaylist(std::vector<std::string> trackIds, lib::spt::api &spotify,
			const lib::cache &cache, QWidget *parent);

		void setTrackIds(std::vector<std::string> trackIds);

	private:
		std::vector<std::string> trackIds;
		lib::spt::api &spotify;
		const lib::cache &cache;

		void onAboutToShow();
		void onTriggered(QAction *action);

		void addToNewPlaylist();
		void addToPlaylist(const lib::spt::playlist &playlist);
	};
}
