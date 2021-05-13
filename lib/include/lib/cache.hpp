#pragma once

#include "lib/format.hpp"
#include "lib/log.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/spotify/album.hpp"

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
		virtual auto get_album_image(const std::string &url)
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
		virtual auto get_playlists() -> std::vector<lib::spt::playlist> = 0;

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
		virtual auto get_playlist(const std::string &id) -> lib::spt::playlist = 0;

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
		virtual auto get_tracks(const std::string &id) -> std::vector<lib::spt::track> = 0;

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
		virtual auto all_tracks() -> std::map<std::string, std::vector<lib::spt::track>> = 0;

		//endregion

		//region lyrics

		/**
		 * Get lyrics HTML
		 * @param track Track
		 * @return Lyrics
		 */
		virtual auto get_lyrics(const lib::spt::track &track) -> std::string = 0;

		/**
		 * Set lyrics HTML
		 * @param track Track
		 * @param lyrics Lyrics to save
		 */
		virtual void set_lyrics(const lib::spt::track &track, const std::string &lyrics) = 0;

		//endregion
	};
}
