#pragma once

#include "icon.hpp"
#include "mainwindow.hpp"

#include <QMenu>
#include <QDesktopServices>

class SongMenu : public QMenu
{
public:
	SongMenu(const QString &trackId, const QString &artist, const QString &name,
		const QString &artistId, const QString &albumId, spt::Spotify *spotify, QWidget *parent = nullptr);
};