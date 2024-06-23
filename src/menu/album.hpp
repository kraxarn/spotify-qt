#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "menu/addtoplaylist.hpp"

#include <QApplication>
#include <QDesktopServices>
#include <QMenu>

namespace Menu
{
	class Album: public QMenu
	{
	Q_OBJECT

	public:
		Album(lib::spt::api &spotify, lib::cache &cache,
			const std::string &albumId, QWidget *parent);

	private:
		bool isLiked = false;

		std::vector<lib::spt::track> tracks;
		lib::spt::album album;
		lib::spt::api &spotify;
		lib::cache &cache;

		QAction *toggleLikedAlbum = nullptr;
		QAction *trackCount = nullptr;
		AddToPlaylist *addToPlaylist = nullptr;

		void setLikedAlbum(bool liked);
		void tracksLoaded();
		auto getTrackIds() const -> std::vector<std::string>;

		void onShuffle(bool checked);
		void onLikeAlbum(bool checked);
		void onCopyLink(bool checked);
		void onCopyName(bool checked);
		void onOpenInSpotify(bool checked);
	};
}
