#pragma once

#include "lib/cache.hpp"
#include "lib/cache/jsoncache.hpp"
#include "lib/paths/paths.hpp"

#include "sqlite3.h"

namespace lib
{
	class db_cache: public cache
	{
	public:
		explicit db_cache(const lib::paths &paths);
		virtual ~db_cache();

		void from_json(const json_cache &json_cache);

		auto get_album_image(const std::string &url) const -> std::vector<unsigned char> override;
		void set_album_image(const std::string &url,
			const std::vector<unsigned char> &data) override;
		auto get_album_image_path(const std::string &url) const -> std::string override;

		auto get_album(const std::string &album_id) const -> lib::spt::album override;
		void set_album(const spt::album &album) override;

		auto get_playlists() const -> std::vector<lib::spt::playlist> override;
		void set_playlists(const std::vector<spt::playlist> &playlists) override;

		auto get_playlist(const std::string &id) const -> lib::spt::playlist override;
		void set_playlist(const spt::playlist &playlist) override;

		auto get_tracks(const std::string &id) const -> std::vector<lib::spt::track> override;
		void set_tracks(const std::string &id, const std::vector<lib::spt::track> &tracks) override;
		auto all_tracks() const -> std::map<std::string, std::vector<lib::spt::track>> override;

		auto get_track_info(const spt::track &track) const -> lib::spt::track_info override;
		void set_track_info(const spt::track &track, const spt::track_info &track_info) override;

		void add_crash(const crash_info &info) override;
		auto get_all_crashes() const -> std::vector<lib::crash_info> override;

	private:
		sqlite3 *db = nullptr;

		auto make_storage(const std::string &path) -> bool;
		auto exec(const char *query, const std::function<void(sqlite3_stmt * )> &callback) -> bool;
	};
}
