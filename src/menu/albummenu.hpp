#pragma once

#include "../mainwindow.hpp"
#include "../spotify/spotify.hpp"

#include <QApplication>
#include <QDesktopServices>
#include <QMenu>

class AlbumMenu: public QMenu
{
Q_OBJECT

public:
	AlbumMenu(spt::Spotify &spotify, const std::string &albumId, QWidget *parent);

private:
	QWidget *parent = nullptr;
	std::vector<lib::spt::track> tracks;
	std::string albumId;
	spt::Spotify &spotify;

	void shuffle(bool checked);
	void shareAlbum(bool checked);
	void shareOpen(bool checked);
};