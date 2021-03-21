#include "lib/json.hpp"

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
