#pragma once

#include "thirdparty/filesystem.hpp"
#include "thirdparty/json.hpp"
#include "lib/paths/paths.hpp"
#include "lib/format.hpp"
#include "lib/log.hpp"
#include "lib/json.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/spotify/album.hpp"

namespace lib
{
	/**
	 * Handles cache
	 */
	class cache
	{
	public:
		/**
		 * Instance a new cache manager, does not create any directories
		 * @param paths Paths to get cache directory
		 */
		explicit cache(const paths &paths);

		//region album

		/**
		 * Get album image data
		 * @param id Album ID
		 * @return Binary JPEG data, or an empty vector if none
		 */
		auto get_album_image(const std::string &url) -> std::vector<unsigned char>;

		/**
		 * Set album image data
		 * @param id Album ID
		 * @param data Binary JPEG data to save
		 */
		void set_album_image(const std::string &url, const std::vector<unsigned char> &data);

		//endregion

		//region playlists

		/**
		 * Get list of user's playlists
		 */
		auto get_playlists() -> std::vector<lib::spt::playlist>;

		/**
		 * Set list of user's playlists
		 */
		void set_playlists(const std::vector<spt::playlist> &playlists);

		//endregion

		//region playlist

		/**
		 * Loads tracks from a cached playlist
		 * @param id Playlist ID
		 * @return Tracks or an empty vector on failure
		 */
		auto get_playlist(const std::string &id) -> lib::spt::playlist;

		/**
		 * Save playlist to cache
		 * @param playlist Playlist to save
		 */
		void set_playlist(const spt::playlist &playlist);

		//endregion

		//region tracks

		/**
		 * Get tracks saved in cache
		 * @param id Id of album for example
		 * @return JSON stored in cache, or an empty object if none
		 */
		auto get_tracks(const std::string &id) -> std::vector<lib::spt::track>;

		/**
		 * Save tracks to cache
		 * @param id Id of album for example
		 * @param tracks Tracks to save
		 */
		void set_tracks(const std::string &id, const std::vector<lib::spt::track> &tracks);

		/**
		 * Get all tracks saved in cache
		 * @return Map as id: tracks
		 */
		auto all_tracks() -> std::map<std::string, std::vector<lib::spt::track>>;

		//endregion

	private:
		const lib::paths &paths;

		/**
		 * Get parent directory for cache type
		 */
		auto dir(const std::string &type) -> ghc::filesystem::path;

		/**
		 * Get file name for id
		 */
		static auto file(const std::string &id, const std::string &extension) -> std::string;

		/**
		 * Get full file path for cache type and id
		 */
		auto path(const std::string &type, const std::string &id,
			const std::string &extension) -> ghc::filesystem::path;

		/**
		 * Get basename of path
		 */
		static auto get_url_id(const ghc::filesystem::path &path) -> std::string;
	};
}
