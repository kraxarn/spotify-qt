#include "lib/cache/jsoncache.hpp"

#include <fstream>

lib::json_cache::json_cache(const lib::paths &paths)
	: paths(paths)
{
}

//region album

auto lib::json_cache::get_album_image(const std::string &url) const -> std::vector<unsigned char>
{
	std::ifstream file(get_album_image_path(url), std::ios::binary);
	if (!file.is_open() || file.bad())
	{
		return {};
	}

	return {
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>(),
	};
}

auto lib::json_cache::get_album_image_path(const std::string &url) const -> std::string
{
	return path("album", get_url_id(url), "");
}

void lib::json_cache::set_album_image(const std::string &url,
	const std::vector<unsigned char> &data)
{
	std::ofstream file(path("album", get_url_id(url), ""),
		std::ios::binary);
	std::copy(data.begin(), data.end(),
		std::ostream_iterator<char>(file));
}

auto lib::json_cache::get_album(const std::string &album_id) const -> lib::spt::album
{
	try
	{
		return lib::json::load(path("albuminfo", album_id, "json"));
	}
	catch (const std::exception &e)
	{
		lib::log::warn("Failed to load album from cache: {}", e.what());
	}

	return {};
}

void lib::json_cache::set_album(const lib::spt::album &album)
{
	lib::json::save(path("albuminfo", album.id, "json"), album);
}

//endregion

//region playlists

auto lib::json_cache::get_playlists() const -> std::vector<lib::spt::playlist>
{
	try
	{
		return json::load(path("playlist", "playlists", "json"));
	}
	catch (const std::exception &e)
	{
		log::warn("Failed to load playlists from cache: {}", e.what());
	}

	return {};
}

void lib::json_cache::set_playlists(const std::vector<spt::playlist> &playlists)
{
	lib::json::save(path("playlist", "playlists", "json"), playlists);
}

//endregion

//region playlist

auto lib::json_cache::get_playlist(const std::string &playlist_id) const -> lib::spt::playlist
{
	try
	{
		return json::load(path("playlist", playlist_id, "json"));
	}
	catch (const std::exception &e)
	{
		log::warn("Failed to load playlist from cache: {}", e.what());
	}

	return {};
}

void lib::json_cache::set_playlist(const spt::playlist &playlist)
{
	lib::json::save(path("playlist", playlist.id, "json"), playlist);
}

//endregion

//region tracks

auto lib::json_cache::get_tracks(const std::string &entity_id) const -> std::vector<lib::spt::track>
{
	const auto tracks_path = path("tracks", entity_id, "json");
	return lib::json::load<std::vector<lib::spt::track>>(tracks_path);
}

void lib::json_cache::set_tracks(const std::string &entity_id,
	const std::vector<lib::spt::track> &tracks)
{
	lib::json::save(path("tracks", entity_id, "json"), tracks);
}

auto lib::json_cache::all_tracks() const -> std::map<std::string, std::vector<lib::spt::track>>
{
	auto dir = paths.cache() / "tracks";
	std::map<std::string, std::vector<lib::spt::track>> results;

	if (!std::filesystem::exists(dir))
	{
		return results;
	}

	for (const auto &entry: std::filesystem::directory_iterator(dir))
	{
		auto entity_id = entry.path().filename().replace_extension().string();
		results[entity_id] = get_tracks(entity_id);
	}

	return results;
}

//endregion

//region lyrics

auto lib::json_cache::get_track_info(const lib::spt::track &track) const -> lib::spt::track_info
{
	return lib::json::load(path("trackInfo", track.id, "json"));
}

void lib::json_cache::set_track_info(const lib::spt::track &track,
	const lib::spt::track_info &track_info)
{
	lib::json::save(path("trackInfo", track.id, "json"), track_info);
}

//endregion

//region crash

void lib::json_cache::add_crash(const lib::crash_info &info)
{
	auto file_name = lib::date_time::now().to_iso_date_time();
	lib::json::save(path("crash", file_name, "json"), info);
}

auto lib::json_cache::get_all_crashes() const -> std::vector<lib::crash_info>
{
	auto dir = std::filesystem::path(paths.cache()) / "crash";
	std::vector<lib::crash_info> results;

	if (!std::filesystem::exists(dir))
	{
		return results;
	}

	for (const auto &entry: std::filesystem::directory_iterator(dir))
	{
		results.push_back(lib::json::load<lib::crash_info>(entry.path()));
	}

	return results;
}

//endregion

//region private

auto lib::json_cache::dir(const std::string &type) const -> std::filesystem::path
{
	auto file_dir = std::filesystem::path(paths.cache()) / type;

	if (!std::filesystem::exists(file_dir))
	{
		std::filesystem::create_directories(file_dir);
	}

	return file_dir;
}

auto lib::json_cache::file(const std::string &entity_id,
	const std::string &extension) -> std::string
{
	return extension.empty()
		? entity_id
		: fmt::format("{}.{}", entity_id, extension);
}

auto lib::json_cache::path(const std::string &type, const std::string &entity_id,
	const std::string &extension) const -> std::string
{
	return (dir(type) / file(entity_id, extension)).string();
}

auto lib::json_cache::get_url_id(const std::filesystem::path &path) -> std::string
{
	return path.stem().string();
}

//endregion
