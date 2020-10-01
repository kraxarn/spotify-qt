#pragma once

#include <utility>

#include "../mainwindow.hpp"
#include "../spotify/spotify.hpp"
#include "../util/icon.hpp"

#include <QDesktopServices>
#include <QMenu>

class SongMenu: public QMenu
{
Q_OBJECT

public:
	SongMenu(QTreeWidgetItem *item, spt::Spotify &spotify, QWidget *parent = nullptr);
	SongMenu(QListWidgetItem *item, QString artist, spt::Spotify &spotify, QWidget *parent = nullptr);

private:
	SongMenu(const QString &trackId, QString artist, QString name, const QString &artistId,
		const QString &albumId, int index, spt::Spotify &spotify, QWidget *parent = nullptr);

	QWidget *parent;
	spt::Spotify &spotify;
	bool isLiked = false;
	const QString trackId;
	const QString artist;
	const QString name; // TODO: trackName
	QString track; // TODO: trackUri
	int index = 0;
	const spt::Playlist *currentPlaylist = nullptr;

	void like(bool checked);
	void addToQueue(bool checked);
	void addToPlaylist(QAction *action);
	void remFromPlaylist(bool checked);
	void openTrackFeatures(bool checked);
	void openLyrics(bool checked);
};