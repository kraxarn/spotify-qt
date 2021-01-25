#include "cache.hpp"

lib::cache::cache(const lib::paths &paths)
	: paths(paths)
{
}

nlohmann::json lib::cache::tracks(const std::string &id)
{
	auto file_path = ghc::filesystem::path(paths.cache())
		/ "tracks"
		/ fmt::format("{}.json", id);

	if (!ghc::filesystem::exists(file_path))
		return nlohmann::json::object();

	std::ifstream file(file_path);
	nlohmann::json json;
	file >> json;
	file.close();
	return json;
}

std::map<std::string, nlohmann::json> lib::cache::all_tracks()
{
	auto dir = ghc::filesystem::path(paths.cache()) / "tracks";
	std::map<std::string, nlohmann::json> results;

	if (!ghc::filesystem::exists(dir))
		return results;

	for (const auto &entry : ghc::filesystem::directory_iterator(dir))
	{
		auto id = entry.path().filename().replace_extension().string();
		results[id] = tracks(id);
	}

	return results;
}
