#pragma once

#include "spotify/playlist.hpp"
#include "mainwindow.hpp"
#include "texteditdialog.hpp"

#include <QMenu>
#include <QUrl>
#include <QRandomGenerator>
#include <QInputDialog>

class PlaylistMenu : public QMenu
{
	Q_OBJECT

public:
	PlaylistMenu(spt::Spotify &spotify, const spt::Playlist &playlist, QWidget *parent = nullptr);
};