#pragma once

#include "../dialog/playlisteditdialog.hpp"
#include "../mainwindow.hpp"
#include "../spotify/playlist.hpp"

#include <QInputDialog>
#include <QMenu>
#include <QRandomGenerator>
#include <QUrl>

class PlaylistMenu : public QMenu
{
	Q_OBJECT

public:
	PlaylistMenu(spt::Spotify &spotify, const spt::Playlist &playlist, QWidget *parent = nullptr);

	PlaylistMenu(spt::Spotify &spotify, const QString &playlistId, QWidget *parent = nullptr);

private:
	PlaylistEditDialog	*editDialog = nullptr;
};