#pragma once

#include "lib/spotify/api.hpp"
#include "lib/strings.hpp"
#include "lib/cache.hpp"

#include "util/icon.hpp"
#include "enum/datarole.hpp"

#include <utility>

#include <QDesktopServices>
#include <QMenu>

class SongMenu: public QMenu
{
Q_OBJECT

public:
	/**
	 * Basic SongMenu
	 */
	SongMenu(const lib::spt::track &track, lib::spt::api &spotify,
		const lib::cache &cache, QWidget *parent);

	/**
	 * SongMenu from artist
	 */
	SongMenu(const lib::spt::track &track, lib::spt::api &spotify,
		const lib::cache &cache, const lib::spt::artist *fromArtist,
		QWidget *parent);

	/**
	 * SongMenu from playlist with index of track in playlist
	 */
	SongMenu(const lib::spt::track &track, lib::spt::api &spotify,
		const lib::cache &cache, int index, QWidget *parent);

private:
	SongMenu(const lib::spt::track &track, lib::spt::api &spotify,
		const lib::cache &cache, const lib::spt::artist *fromArtist,
		int index, QWidget *parent);

	lib::spt::api &spotify;
	bool isLiked = false;

	lib::spt::track track;
	std::string trackUri;
	int index = 0;
	lib::spt::playlist currentPlaylist;
	QAction *toggleLiked = nullptr;

	auto getTrackUrl() const -> QString;
	void viewArtist(const lib::spt::entity &artist);
	void setLiked(bool liked);

	void addToPlaylist(const std::string &playlistId) const;
	void addToNewPlaylist();

	void onAudioFeatures(bool checked);
	void onLyrics(bool checked);
	void onCopySongLink(bool checked);
	void onOpenInSpotify(bool checked);
	void onLike(bool checked);
	void onAddToQueue(bool checked);
	void onAddToPlaylist(QAction *action);
	void onRemoveFromPlaylist(bool checked);
	void onOpenAlbum(bool checked);
};
