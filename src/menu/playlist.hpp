#pragma once

#include "dialog/playlisteditdialog.hpp"
#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "lib/random.hpp"

#include <QInputDialog>
#include <QMenu>
#include <QUrl>

namespace Menu
{
	class Playlist: public QMenu
	{
	Q_OBJECT

	public:
		Playlist(lib::spt::api &spotify, const lib::spt::playlist &playlist, lib::cache &cache,
			QWidget *parent = nullptr);

	private:
		lib::spt::playlist playlist;
		lib::cache &cache;

		PlaylistEditDialog *editDialog = nullptr;
		std::vector<lib::spt::track> tracks;
		QAction *tracksAction = nullptr;
		QAction *byAction = nullptr;
		QAction *editAction = nullptr;

		void tracksLoaded(const std::vector<lib::spt::track> &items);
	};
}
