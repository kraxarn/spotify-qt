#include "lib/json.hpp"

nlohmann::json lib::json::combine(const nlohmann::json &item1, const nlohmann::json &item2)
{
	auto item = nlohmann::json::array();

	if (item1.is_array())
		item.insert(item.end(), item1.begin(), item1.end());
	if (item2.is_array())
		item.insert(item.end(), item2.begin(), item2.end());

	return item;
}

nlohmann::json lib::json::load_json(const ghc::filesystem::path &path)
{
	std::ifstream file(path);
	if (!file.is_open() || file.bad())
	{
		// File not found errors fail silently
		return nlohmann::json();
	}

	nlohmann::json json;
	file >> json;
	return json;
}

void lib::json::save_json(const ghc::filesystem::path &path, const nlohmann::json &json)
{
	std::ofstream file(path);
	file << std::setw(4) << json;
}
