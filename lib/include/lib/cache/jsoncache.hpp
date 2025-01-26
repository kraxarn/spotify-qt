#pragma once

#include "lib/cache.hpp"
#include "lib/filesystem.hpp"
#include "lib/paths/paths.hpp"

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

		auto get_album_image(const std::string &url) const -> std::vector<unsigned char> override;
		auto get_album_image_path(const std::string &url) const -> std::string override;
		void set_album_image(const std::string &url,
			const std::vector<unsigned char> &data) override;

		auto get_album(const std::string &album_id) const -> lib::spt::album override;
		void set_album(const spt::album &album) override;

		auto get_playlists() const -> std::vector<lib::spt::playlist> override;
		void set_playlists(const std::vector<spt::playlist> &playlists) override;

		auto get_playlist(const std::string &playlist_id) const -> lib::spt::playlist override;
		void set_playlist(const spt::playlist &playlist) override;

		auto get_tracks(const std::string &entity_id) const -> std::vector<lib::spt::track> override;
		void set_tracks(const std::string &entity_id,
			const std::vector<lib::spt::track> &tracks) override;
		auto all_tracks() const -> std::map<std::string, std::vector<lib::spt::track>> override;

		auto get_track_info(const lib::spt::track &track) const -> lib::spt::track_info override;
		void set_track_info(const lib::spt::track &track,
			const lib::spt::track_info &track_info) override;

		void add_crash(const lib::crash_info &info) override;
		auto get_all_crashes() const -> std::vector<lib::crash_info> override;

	private:
		const lib::paths &paths;

		/**
		 * Get parent directory for cache type
		 */
		auto dir(const std::string &type) const -> std::filesystem::path;

		/**
		 * Get file name for id
		 */
		static auto file(const std::string &entity_id, const std::string &extension) -> std::string;

		/**
		 * Get full file path for cache type and id
		 */
		auto path(const std::string &type, const std::string &entity_id,
			const std::string &extension) const -> std::string;

		/**
		 * Get basename of path
		 */
		static auto get_url_id(const std::filesystem::path &path) -> std::string;
	};
}
