#pragma once

#include "../view/playlisteditview.hpp"
#include "../mainwindow.hpp"
#include "../spotify/playlist.hpp"

#include <QInputDialog>
#include <QMenu>
#include <QUrl>

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#endif

class PlaylistMenu: public QMenu
{
Q_OBJECT

public:
	PlaylistMenu(spt::Spotify &spotify, const spt::Playlist &playlist, QWidget *parent = nullptr);

	PlaylistMenu(spt::Spotify &spotify, const QString &playlistId, QWidget *parent = nullptr);

private:
	PlaylistEditView *editDialog = nullptr;
	QWidget *parent = nullptr;
	const spt::Playlist playlist;
};