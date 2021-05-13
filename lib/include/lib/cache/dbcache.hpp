#pragma once
#ifdef LIB_SQLITE

#include "lib/cache.hpp"
#include "lib/paths/paths.hpp"
#include "lib/spotify/callback.hpp"

#include <sqlite3.h>

namespace lib
{
	/** Result from database with column results */
	using sql_callback = lib::callback<std::vector<std::string>>;

	/** SQLite cache */
	class db_cache: public cache
	{
	public:
		/**
		 * Create new instance and open/create new database
		 * @param paths Path to database
		 */
		explicit db_cache(const lib::paths &paths);
		~db_cache();

		auto get_album_image(const std::string &url) -> std::vector<unsigned char> override;
		void set_album_image(const std::string &url,
			const std::vector<unsigned char> &data) override;

		auto get_playlists() -> std::vector<lib::spt::playlist> override;
		void set_playlists(const std::vector<spt::playlist> &playlists) override;

		auto get_playlist(const std::string &id) -> lib::spt::playlist override;
		void set_playlist(const spt::playlist &playlist) override;

		auto get_tracks(const std::string &id) -> std::vector<lib::spt::track> override;
		void set_tracks(const std::string &id, const std::vector<lib::spt::track> &tracks) override;
		auto all_tracks() -> std::map<std::string, std::vector<lib::spt::track>> override;

		auto get_lyrics(const spt::track &track) -> std::string override;
		void set_lyrics(const spt::track &track, const std::string &lyrics) override;

	private:
		sqlite3 *db = nullptr;

		/** Latest database version */
		static constexpr unsigned short database_version = 1;

		static auto db_callback(void *data, int argc,
			char **argv, char **col_name) -> int;

		auto open(const lib::paths &paths) -> bool;
		void init();

		auto exec(const char *query, lib::sql_callback &callback) -> bool;
		auto exec(const char *query) -> bool;

		/**
		 * Get items, often with a select
		 * @tparam T Type of items to collect
		 * @param query Full SQL query
		 * @param transform How to transform SQL row to entity
		 * @return Entities
		 */
		template<typename T>
		auto select(const std::string &query,
			std::function<T(sqlite3_stmt &)> &transform) -> std::vector<T>
		{
			sqlite3_stmt *stmt = nullptr;
			std::vector<T> items;

			auto status = sqlite3_prepare_v2(db, query.c_str(), (int) query.size(),
				&stmt, nullptr);
			if (status != SQLITE_OK)
			{
				lib::log::error("SQLite: {}", sqlite3_errmsg(db));
				return items;
			}

			int step = 0;
			while (step != SQLITE_DONE)
			{
				items.push_back(transform(*stmt));
				step = sqlite3_step(stmt);
			}

			sqlite3_reset(stmt);
			return items;
		}
	};
}

#endif
