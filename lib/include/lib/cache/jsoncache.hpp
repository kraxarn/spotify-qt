#pragma once

#include "lib/cache.hpp"
#include "lib/json.hpp"
#include "lib/paths/paths.hpp"
#include "thirdparty/filesystem.hpp"
#include "thirdparty/json.hpp"

namespace lib
{
	/**
	 * Cache as JSON files
	 */
	class json_cache: public cache
	{
	public:
		/**
		 * Instance a new json cache manager, does not create any directories
		 * @param paths Paths to get cache directory
		 */
		explicit json_cache(const paths &paths);

		auto get_album_image(const std::string &url) -> std::vector<unsigned char> override;
		void set_album_image(const std::string &url,
			const std::vector<unsigned char> &data) override;

		auto get_playlists() -> std::vector<lib::spt::playlist> override;
		void set_playlists(const std::vector<spt::playlist> &playlists) override;

		auto get_playlist(const std::string &id) -> lib::spt::playlist override;
		void set_playlist(const spt::playlist &playlist) override;

		auto get_tracks(const std::string &id) -> std::vector<lib::spt::track> override;
		void set_tracks(const std::string &id,
			const std::vector<lib::spt::track> &tracks) override;
		auto all_tracks() -> std::map<std::string, std::vector<lib::spt::track>> override;

		auto get_lyrics(const lib::spt::track &track) -> std::string override;
		void set_lyrics(const lib::spt::track &track, const std::string &lyrics) override;

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
