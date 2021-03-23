#pragma once

#include <utility>

#include "../mainwindow.hpp"
#include "../spotify/spotify.hpp"
#include "../util/icon.hpp"
#include "lib/strings.hpp"

#include <QDesktopServices>
#include <QMenu>

class SongMenu: public QMenu
{
Q_OBJECT

public:
	SongMenu(QTreeWidgetItem *item, spt::Spotify &spotify, QWidget *parent);

	SongMenu(QListWidgetItem *item, std::string artist, spt::Spotify &spotify, QWidget *parent);

	SongMenu(const lib::spt::track &track, spt::Spotify &spotify, QWidget *parent);

private:
	SongMenu(const std::string &trackId, std::string artist, std::string name, std::string artistId,
		std::string albumId, int index, spt::Spotify &spotify, QWidget *parent);

	spt::Spotify &spotify;
	bool isLiked = false;
	const std::string trackId;
	const std::string artist;
	const std::string artistId;
	const std::string albumId;
	const std::string trackName;
	QString trackUri;
	int index = 0;
	const lib::spt::playlist *currentPlaylist = nullptr;

	void like(bool checked);
	void addToQueue(bool checked);
	void addToPlaylist(QAction *action);
	void remFromPlaylist(bool checked);
	void openTrackFeatures(bool checked);
	void openLyrics(bool checked);
	void viewArtist(bool checked);
	void openAlbum(bool checked);
};