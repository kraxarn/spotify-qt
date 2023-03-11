#include "lib/cache/dbcache.hpp"
#include "lib/stopwatch.hpp"
#include "model_cache.hpp"

lib::db_cache::db_cache(const lib::paths &paths)
{
	lib::stopwatch stopwatch;
	stopwatch.start();

	if (!make_storage(paths.cache() / "cache.sqlite"))
	{
		lib::log::debug("Schema sync failed");
		return;
	}

	stopwatch.stop();
	lib::log::debug("Synced schema in {} ms",
		stopwatch.elapsed<lib::stopwatch::ms, long>());
}

lib::db_cache::~db_cache()
{
	sqlite3_close(db);
}

auto lib::db_cache::make_storage(const std::string &path) -> bool
{
	if (sqlite3_open(path.c_str(), &db) != SQLITE_OK)
	{
		lib::log::debug("Failed to open database: {}", sqlite3_errmsg(db));
		return false;
	}

	return exec(model_cache_sql, {});
}

auto lib::db_cache::exec(const char *query,
	const std::function<void(sqlite3_stmt *)> &callback) -> bool
{
	sqlite3_stmt *stmt = nullptr;

	const auto prepare = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
	if (prepare != SQLITE_OK)
	{
		lib::log::error("Failed to prepare query: {}", sqlite3_errmsg(db));
		return false;
	}

	int step;
	while ((step = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		callback(stmt);
	}
	sqlite3_finalize(stmt);

	if (step != SQLITE_DONE)
	{
		lib::log::error("Failed to step: {}", sqlite3_errmsg(db));
		return false;
	}

	return true;
}

void lib::db_cache::from_json(const lib::json_cache &json_cache)
{

}

auto lib::db_cache::get_album_image(const std::string &url) const -> std::vector<unsigned char>
{
	return {};
}

void lib::db_cache::set_album_image(const std::string &url, const std::vector<unsigned char> &data)
{
}

std::string lib::db_cache::get_album_image_path(const std::string &url) const
{
	return {};
}

lib::spt::album lib::db_cache::get_album(const std::string &album_id) const
{
	return {};
}

void lib::db_cache::set_album(const lib::spt::album &album)
{
}

auto lib::db_cache::get_playlists() const -> std::vector<lib::spt::playlist>
{
	return {};
}

void lib::db_cache::set_playlists(const std::vector<spt::playlist> &playlists)
{

}

auto lib::db_cache::get_playlist(const std::string &id) const -> lib::spt::playlist
{
	return {};
}

void lib::db_cache::set_playlist(const lib::spt::playlist &playlist)
{

}

auto lib::db_cache::get_tracks(const std::string &id) const -> std::vector<lib::spt::track>
{
	return {};
}

void lib::db_cache::set_tracks(const std::string &id, const std::vector<lib::spt::track> &tracks)
{

}

auto lib::db_cache::all_tracks() const -> std::map<std::string, std::vector<lib::spt::track>>
{
	return {};
}

auto lib::db_cache::get_track_info(const lib::spt::track &track) const -> lib::spt::track_info
{
	return {};
}

void lib::db_cache::set_track_info(const lib::spt::track &track,
	const lib::spt::track_info &track_info)
{

}

void lib::db_cache::add_crash(const lib::crash_info &info)
{

}

auto lib::db_cache::get_all_crashes() const -> std::vector<lib::crash_info>
{
	return {};
}
