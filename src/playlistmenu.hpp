#pragma once

#include "spotify/playlist.hpp"
#include "mainwindow.hpp"

#include <QMenu>
#include <QUrl>

class PlaylistMenu : public QMenu
{
	Q_OBJECT

public:
	PlaylistMenu(spt::Spotify &spotify, spt::Playlist playlist, QWidget *parent = nullptr);
};