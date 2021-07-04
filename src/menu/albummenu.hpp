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
	AlbumMenu(spt::Spotify &spotify, lib::cache &cache, const std::string &albumId,
		QWidget *parent);

private:
	QWidget *parent = nullptr;
	std::vector<lib::spt::track> tracks;
	std::string albumId;
	spt::Spotify &spotify;
	QAction *trackCount = nullptr;

	void tracksLoaded(const std::vector<lib::spt::track> &items);

	void shuffle(bool checked);
	void shareAlbum(bool checked);
	void shareOpen(bool checked);
};
