#pragma once

#include "../dialog/playlisteditdialog.hpp"
#include "../mainwindow.hpp"
#include "../spotify/playlist.hpp"
#include "lib/random.hpp"

#include <QInputDialog>
#include <QMenu>
#include <QUrl>

class PlaylistMenu: public QMenu
{
Q_OBJECT

public:
	PlaylistMenu(spt::Spotify &spotify, const spt::Playlist &playlist, QWidget *parent = nullptr);

	PlaylistMenu(spt::Spotify &spotify, const QString &playlistId, QWidget *parent = nullptr);

private:
	PlaylistEditDialog *editDialog = nullptr;
	QWidget *parent = nullptr;
	const spt::Playlist playlist;
};