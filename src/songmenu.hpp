#pragma once

#include "icon.hpp"
#include "mainwindow.hpp"
#include "spotify/spotify.hpp"

#include <QDesktopServices>
#include <QMenu>

class SongMenu: public QMenu
{
Q_OBJECT

public:
	SongMenu(
		const QString &trackId, const QString &artist, const QString &name,
		const QString &artistId, const QString &albumId, spt::Spotify &spotify, QWidget *parent = nullptr);

private:
	QWidget *parent;
	spt::Spotify &spotify;
	bool isLiked;
	const QString &trackId;
	const QString &artist;
	const QString &name;
	QString track;
	const spt::Playlist *currentPlaylist = nullptr;

	void like(bool checked);
	void addToQueue(bool checked);
	void addToPlaylist(QAction *action);
	void remFromPlaylist(bool checked);
	void openTrackFeatures(bool checked);
	void openLyrics(bool checked);
};