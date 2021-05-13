
#include "lib/cache/jsoncache.hpp"

lib::json_cache::json_cache(const lib::paths &paths)
	: paths(paths),
	cache()
{
}

//region album

auto lib::json_cache::get_album_image(const std::string &url) -> std::vector<unsigned char>
{
	std::ifstream file(path("album", get_url_id(url), ""),
		std::ios::binary);
	if (!file.is_open() || file.bad())
	{
		return std::vector<unsigned char>();
	}

	return std::vector<unsigned char>(std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>());
}

void lib::json_cache::set_album_image(const std::string &url,
	const std::vector<unsigned char> &data)
{
	std::ofstream file(path("album", get_url_id(url), ""),
		std::ios::binary);
	std::copy(data.begin(), data.end(),
		std::ostream_iterator<char>(file));
}

//endregion

//region playlists

auto lib::json_cache::get_playlists() -> std::vector<lib::spt::playlist>
{
	try
	{
		return json::load(path("playlist", "playlists", "json"));
	}
	catch (const std::exception &e)
	{
		log::warn("Failed to load playlists from cache: {}", e.what());
	}

	return std::vector<lib::spt::playlist>();
}

void lib::json_cache::set_playlists(const std::vector<spt::playlist> &playlists)
{
	lib::json::save(path("playlist", "playlists", "json"), playlists);
}

//endregion

//region playlist

auto lib::json_cache::get_playlist(const std::string &id) -> lib::spt::playlist
{
	try
	{
		return json::load(path("playlist", id, "json"));
	}
	catch (const std::exception &e)
	{
		log::warn("Failed to load playlist from cache: {}", e.what());
	}

	return lib::spt::playlist();
}

void lib::json_cache::set_playlist(const spt::playlist &playlist)
{
	lib::json::save(path("playlist", playlist.id, "json"), playlist);
}

//endregion

//region tracks

auto lib::json_cache::get_tracks(const std::string &id) -> std::vector<lib::spt::track>
{
	return lib::json::load<std::vector<lib::spt::track>>(path("tracks", id, "json"));
}

void lib::json_cache::set_tracks(const std::string &id, const std::vector<lib::spt::track> &tracks)
{
	lib::json::save(path("tracks", id, "json"), tracks);
}

auto lib::json_cache::all_tracks() -> std::map<std::string, std::vector<lib::spt::track>>
{
	auto dir = ghc::filesystem::path(paths.cache()) / "tracks";
	std::map<std::string, std::vector<lib::spt::track>> results;

	if (!ghc::filesystem::exists(dir))
	{
		return results;
	}

	for (const auto &entry : ghc::filesystem::directory_iterator(dir))
	{
		auto id = entry.path().filename().replace_extension().string();
		results[id] = get_tracks(id);
	}

	return results;
}

//endregion

//region lyrics

auto lib::json_cache::get_lyrics(const lib::spt::track &track) -> std::string
{
	std::ifstream file(path("lyrics", track.id, "html"));
	if (!file.is_open() || file.bad())
	{
		return std::string();
	}

	std::ostringstream stream;
	stream << file.rdbuf();
	return stream.str();
}

void lib::json_cache::set_lyrics(const lib::spt::track &track, const std::string &lyrics)
{
	std::ofstream file(path("lyrics", track.id, "html"));
	file << lyrics;
}

//endregion

//region private

auto lib::json_cache::dir(const std::string &type) -> ghc::filesystem::path
{
	auto file_dir = ghc::filesystem::path(paths.cache()) / type;

	if (!ghc::filesystem::exists(file_dir))
	{
		ghc::filesystem::create_directories(file_dir);
	}

	return file_dir;
}

auto lib::json_cache::file(const std::string &id, const std::string &extension) -> std::string
{
	return extension.empty()
		? id
		: fmt::format("{}.{}", id, extension);
}

auto lib::json_cache::path(const std::string &type, const std::string &id,
	const std::string &extension) -> ghc::filesystem::path
{
	return dir(type) / file(id, extension);
}

auto lib::json_cache::get_url_id(const ghc::filesystem::path &path) -> std::string
{
	return path.stem();
}

//endregion
