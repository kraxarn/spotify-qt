#include "lib/cache/dbcache.hpp"
#ifdef LIB_SQLITE

lib::db_cache::db_cache(const lib::paths &paths)
{
	open(paths);
}

lib::db_cache::~db_cache()
{
	if (db != nullptr)
	{
		sqlite3_close(db);
	}
}

//region private

auto lib::db_cache::db_callback(void *data, int argc,
	char **argv, char **/*col_names*/) -> int
{
	const auto *callback = data == nullptr
		? nullptr
		: (lib::sql_callback *) data;

	std::vector<std::string> results;
	results.reserve(argc);
	for (auto i = 0; i < argc; i++)
	{
		results.emplace_back(argv[i]);
	}

	if (callback != nullptr)
	{
		(*callback)(results);
	}
	return 0;
}

auto lib::db_cache::open(const lib::paths &paths) -> bool
{
	auto result = sqlite3_open((paths.cache() / "cache.db").c_str(), &db);
	if (result == SQLITE_OK)
	{
		return true;
	}

	lib::log::error("Failed to open cache database: {}",
		sqlite3_errmsg(db));
	return false;
}

void lib::db_cache::init()
{
	if (db == nullptr)
	{
		return;
	}

	// Metadata
	exec("create table if not exists Metadata (int version)");
}

auto lib::db_cache::exec(const char *query, lib::sql_callback &callback) -> bool
{
	char *err = nullptr;

	auto result = sqlite3_exec(db, query, &lib::db_cache::db_callback,
		callback ? (void *) &callback : nullptr, &err);

	if (result == SQLITE_OK)
	{
		return true;
	}

	lib::log::error("SQLite: {}", (const char *) err);
	sqlite3_free(err);
	return false;
}

auto lib::db_cache::exec(const char *query) -> bool
{
	return exec(query, {});
}

//endregion

//region album

auto lib::db_cache::get_album_image(const std::string &url) -> std::vector<unsigned char>
{
	return std::vector<unsigned char>();
}

void lib::db_cache::set_album_image(const std::string &url, const std::vector<unsigned char> &data)
{

}

//endregion

//region playlists

std::vector<lib::spt::playlist> lib::db_cache::get_playlists()
{
	return std::vector<lib::spt::playlist>();
}

void lib::db_cache::set_playlists(const std::vector<spt::playlist> &playlists)
{

}

//endregion

//region playlist

lib::spt::playlist lib::db_cache::get_playlist(const std::string &id)
{
	return lib::spt::playlist();
}

void lib::db_cache::set_playlist(const lib::spt::playlist &playlist)
{

}

//endregion

//region tracks

std::vector<lib::spt::track> lib::db_cache::get_tracks(const std::string &id)
{
	return std::vector<lib::spt::track>();
}

void lib::db_cache::set_tracks(const std::string &id, const std::vector<lib::spt::track> &tracks)
{

}

std::map<std::string, std::vector<lib::spt::track>> lib::db_cache::all_tracks()
{
	return std::map<std::string, std::vector<lib::spt::track>>();
}

//endregion

//region lyrics

std::string lib::db_cache::get_lyrics(const lib::spt::track &track)
{
	return std::string();
}

void lib::db_cache::set_lyrics(const lib::spt::track &track, const std::string &lyrics)
{

}

//endregion

#endif
