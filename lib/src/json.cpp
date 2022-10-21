#include "lib/json.hpp"

auto lib::json::combine(const nlohmann::json &item1, const nlohmann::json &item2) -> nlohmann::json
{
	auto item = nlohmann::json::array();

	if (item1.is_array())
	{
		item.insert(item.end(), item1.begin(), item1.end());
	}
	if (item2.is_array())
	{
		item.insert(item.end(), item2.begin(), item2.end());
	}

	return item;
}

auto lib::json::load(const ghc::filesystem::path &path) -> nlohmann::json
{
	ghc::filesystem::ifstream file(path);
	if (!file.is_open() || file.bad())
	{
		// File not found errors fail silently
		return {};
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

	return {};
}

void lib::json::save(const ghc::filesystem::path &path, const nlohmann::json &json)
{
	try
	{
		ghc::filesystem::ofstream file(path);
		file << std::setw(4) << json;
	}
	catch (const std::exception &e)
	{
		log::warn("Failed to save items to \"{}\": {}",
			path.string(), e.what());
	}
}

void lib::json::find_item(const std::string &name, const nlohmann::json &json, std::string &item)
{
	const auto &singular = name;
	const auto &plural = lib::fmt::format("{}s", singular);

	if (json.contains(plural))
	{
		const auto &items = json.at(plural);
		if (items.is_array() && !items.empty())
		{
			const auto &last_item = items.back();
			if (last_item.is_string())
			{
				last_item.get_to(item);
			}
			else if (last_item.contains("url"))
			{
				// Assume URL is a string
				last_item.at("url").get_to(item);
			}
		}
	}
	else if (json.contains(singular))
	{
		json.at(singular).get_to(item);
	}
}
