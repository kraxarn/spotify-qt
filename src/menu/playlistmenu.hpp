#pragma once

#include "../dialog/playlisteditdialog.hpp"
#include "../mainwindow.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/random.hpp"

#include <QInputDialog>
#include <QMenu>
#include <QUrl>

class PlaylistMenu: public QMenu
{
Q_OBJECT

public:
	PlaylistMenu(spt::Spotify &spotify, const lib::spt::playlist &playlist, lib::cache &cache,
		QWidget *parent = nullptr);

private:
	PlaylistEditDialog *editDialog = nullptr;
	QWidget *parent = nullptr;
	const lib::spt::playlist playlist;
	std::vector<lib::spt::track> tracks;
	QAction *tracksAction = nullptr;
	QAction *byAction = nullptr;
	QAction *editAction = nullptr;

	void tracksLoaded(const std::vector<lib::spt::track> &items);
};
