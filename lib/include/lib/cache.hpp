#pragma once

#include "lib/format.hpp"
#include "lib/log.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/spotify/album.hpp"
#include "lib/spotify/trackinfo.hpp"
#include "lib/crash/crashinfo.hpp"

namespace lib
{
	/**
	 * Abstract cache handler
	 */
	class cache
	{
	public:
		/**
		 * Instance abstract cache
		 */
		cache() = default;

		//region album

		/**
		 * Get album image data
		 * @param id Album ID
		 * @return Binary JPEG data, or an empty vector if none
		 */
		virtual auto get_album_image(const std::string &url) const
		-> std::vector<unsigned char> = 0;

		/**
		 * Set album image data
		 * @param id Album ID
		 * @param data Binary JPEG data to save
		 */
		virtual void set_album_image(const std::string &url,
			const std::vector<unsigned char> &data) = 0;

		//endregion

		//region playlists

		/**
		 * Get list of user's playlists
		 */
		virtual auto get_playlists() const -> std::vector<lib::spt::playlist> = 0;

		/**
		 * Set list of user's playlists
		 */
		virtual void set_playlists(const std::vector<spt::playlist> &playlists) = 0;

		//endregion

		//region playlist

		/**
		 * Loads tracks from a cached playlist
		 * @param id Playlist ID
		 * @return Tracks or an empty vector on failure
		 */
		virtual auto get_playlist(const std::string &id) const -> lib::spt::playlist = 0;

		/**
		 * Save playlist to cache
		 * @param playlist Playlist to save
		 */
		virtual void set_playlist(const spt::playlist &playlist) = 0;

		//endregion

		//region tracks

		/**
		 * Get tracks saved in cache
		 * @param id Id of album for example
		 * @return JSON stored in cache, or an empty object if none
		 */
		virtual auto get_tracks(const std::string &id) const -> std::vector<lib::spt::track> = 0;

		/**
		 * Save tracks to cache
		 * @param id Id of album for example
		 * @param tracks Tracks to save
		 */
		virtual void set_tracks(const std::string &id,
			const std::vector<lib::spt::track> &tracks) = 0;

		/**
		 * Get all tracks saved in cache
		 * @return Map as id: tracks
		 */
		virtual auto all_tracks() const -> std::map<std::string, std::vector<lib::spt::track>> = 0;

		//endregion

		//region lyrics

		/**
		 * Get track info
		 * @param track Track
		 * @return Track info
		 */
		virtual auto get_track_info(const lib::spt::track &track) const -> lib::spt::track_info = 0;

		/**
		 * Set track info
		 * @param track Track
		 * @param track_info Track info
		 */
		virtual void set_track_info(const lib::spt::track &track,
			const lib::spt::track_info &track_info) = 0;

		//endregion

		//region crash

		/**
		 * Add crash log
		 * @param info Crash information
		 */
		virtual void add_crash(const lib::crash_info &info) = 0;

		/**
		 * Get all known crashes
		 * @return Crash infos
		 */
		virtual auto get_all_crashes() const -> std::vector<lib::crash_info> = 0;

		//endregion
	};
}
