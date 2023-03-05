#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

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
		std::vector<lib::spt::track> tracks;
		lib::spt::album album;
		lib::spt::api &spotify;
		lib::cache &cache;

		QAction *trackCount = nullptr;
		QMenu *addToPlaylist = nullptr;

		void tracksLoaded(const std::vector<lib::spt::track> &items);
		auto getTrackIds() const -> std::vector<std::string>;

		void onShuffle(bool checked);
		void onCopyLink(bool checked);
		void onCopyName(bool checked);
		void onOpenInSpotify(bool checked);
	};
}
