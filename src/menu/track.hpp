#pragma once

#include "lib/spotify/api.hpp"
#include "lib/strings.hpp"
#include "lib/cache.hpp"

#include "util/icon.hpp"
#include "enum/datarole.hpp"

#include <utility>

#include <QDesktopServices>
#include <QMenu>

using PlaylistTrack = QPair<int, lib::spt::track>;

namespace Menu
{
	class Track: public QMenu
	{
	Q_OBJECT

	public:
		/**
		 * Basic track menu for a single track
		 */
		Track(const lib::spt::track &track, lib::spt::api &spotify,
			const lib::cache &cache, QWidget *parent);

		/**
		 * Track menu from playlist with indexes of tracks in playlist
		 */
		Track(const QList<PlaylistTrack> &tracks, lib::spt::api &spotify,
			const lib::cache &cache, QWidget *parent);

		/**
		 * Track menu from artist
		 */
		Track(const QList<PlaylistTrack> &tracks, lib::spt::api &spotify,
			const lib::cache &cache, const lib::spt::artist *fromArtist, QWidget *parent);

	private:
		lib::spt::api &spotify;
		bool isLiked = false;

		lib::spt::playlist currentPlaylist;
		QAction *toggleLiked = nullptr;

		QList<PlaylistTrack> tracks;

		auto getTrackUrl() const -> QString;
		auto getTrackDisplayName() const -> QString;
		void viewArtist(const lib::spt::entity &artist);
		void setLiked(bool liked);

		void addToQueue(const QList<PlaylistTrack>::const_iterator &begin,
			const QList<PlaylistTrack>::const_iterator &end);

		auto getRemoveFromPlaylistAction(const std::string &currentUserId) -> QAction *;
		auto getArtistObject(const lib::spt::artist *fromArtist) -> QObject *;
		auto getAlbumAction() -> QAction *;

		auto anyInPlaylist() const -> bool;
		auto getTrackIds() const -> std::vector<std::string>;
		auto getTracks() const -> std::vector<lib::spt::track>;

		/** All tracks have the same artists */
		auto allSameArtists() const -> bool;

		/** All tracks have the same album */
		auto allSameAlbum() const -> bool;

		void onLyrics(bool checked);
		void onCopySongLink(bool checked);
		void onCopySongName(bool checked);
		void onOpenInSpotify(bool checked);
		void onLike(bool checked);
		void onAddToQueue(bool checked);
		void onRemoveFromPlaylist(bool checked);
		void onOpenAlbum(bool checked);
	};
}
