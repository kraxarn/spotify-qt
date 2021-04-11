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
	SongMenu(QTreeWidgetItem *item, spt::Spotify &spotify, QWidget *parent);

	SongMenu(QListWidgetItem *item, const std::vector<lib::spt::entity> &artists,
		spt::Spotify &spotify, bool forceArtistSubmenu = false, QWidget *parent = nullptr);

	SongMenu(const lib::spt::track &track, spt::Spotify &spotify, QWidget *parent);

private:
	SongMenu(const std::string &trackId, const std::vector<lib::spt::entity> &artists,
		std::string name, std::string albumId, int index, spt::Spotify &spotify,
		bool forceArtistSubmenu, QWidget *parent);

	spt::Spotify &spotify;
	bool isLiked = false;

	std::vector<lib::spt::entity> artists;
	const std::string trackId;
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
	void viewArtist(const lib::spt::entity &artist);
	void openAlbum(bool checked);
};