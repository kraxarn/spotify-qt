#pragma once

#include "../spotify/spotify.hpp"
#include "../util/icon.hpp"
#include "lib/strings.hpp"
#include "enum/datarole.hpp"

#include <utility>

#include <QDesktopServices>
#include <QMenu>

class SongMenu: public QMenu
{
Q_OBJECT

public:
	SongMenu(const lib::spt::track &track, spt::Spotify &spotify, QWidget *parent);

	SongMenu(const lib::spt::track &track, spt::Spotify &spotify,
		bool forceArtistSubmenu, QWidget *parent);

private:
	SongMenu(const lib::spt::track &track, int index, spt::Spotify &spotify,
		bool forceArtistSubmenu, QWidget *parent);

	spt::Spotify &spotify;
	bool isLiked = false;

	const lib::spt::track &track;
	std::string trackUri;
	int index = 0;
	const lib::spt::playlist *currentPlaylist = nullptr;
	QAction *toggleLiked = nullptr;

	auto getTrackUrl() const -> QString;

	void like(bool checked);
	void addToQueue(bool checked);
	void addToPlaylist(QAction *action);
	void remFromPlaylist(bool checked);
	void openTrackFeatures(bool checked);
	void openLyrics(bool checked);
	void viewArtist(const lib::spt::entity &artist);
	void openAlbum(bool checked);
	void setLiked(bool liked);
};