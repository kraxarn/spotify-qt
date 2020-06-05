#pragma once

#include "spotify/playlist.hpp"
#include "mainwindow.hpp"
#include "playlisteditdialog.hpp"

#include <QMenu>
#include <QUrl>
#include <QRandomGenerator>
#include <QInputDialog>

class PlaylistMenu : public QMenu
{
	Q_OBJECT

public:
	PlaylistMenu(spt::Spotify &spotify, const spt::Playlist &playlist, QWidget *parent = nullptr);

	PlaylistMenu(spt::Spotify &spotify, const QString &playlistId, QWidget *parent = nullptr);

private:
	PlaylistEditDialog	*editDialog = nullptr;
};