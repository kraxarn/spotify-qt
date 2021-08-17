#include "lib/json.hpp"

auto lib::json::combine(const nlohmann::json &item1, const nlohmann::json &item2) -> nlohmann::json
{
	auto item = nlohmann::json::array();

	if (item1.is_array())
		item.insert(item.end(), item1.begin(), item1.end());
	if (item2.is_array())
		item.insert(item.end(), item2.begin(), item2.end());

	return item;
}

auto lib::json::load(const ghc::filesystem::path &path) -> nlohmann::json
{
	std::ifstream file(path);
	if (!file.is_open() || file.bad())
	{
		// File not found errors fail silently
		return nlohmann::json();
	}

	try
	{
		nlohmann::json json;
		file >> json;
		return json;
	}
	catch (const std::exception &e)
	{
		log::warn("Failed to load items from \"{}\": {}",
			path.string(), e.what());
	}

	return nlohmann::json();
}

void lib::json::save(const ghc::filesystem::path &path, const nlohmann::json &json)
{
	try
	{
		std::ofstream file(path);
		file << std::setw(4) << json;
	}
	catch (const std::exception &e)
	{
		log::warn("Failed to save items to \"{}\": {}",
			path.string(), e.what());
	}
}
