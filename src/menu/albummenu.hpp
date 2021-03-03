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
	AlbumMenu(spt::Spotify &spotify, const QString &albumId, QWidget *parent);

private:
	QWidget *parent = nullptr;
	QVector<spt::Track> tracks;
	QString albumId;
	spt::Spotify &spotify;

	QAction *trackCount = nullptr;
	QAction *playShuffle = nullptr;

	void tracksLoaded(const QVector<spt::Track> &result);

	void shuffle(bool checked);
	void shareAlbum(bool checked);
	void shareOpen(bool checked);
};