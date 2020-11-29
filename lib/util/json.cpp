#include "json.hpp"

nlohmann::json lib::json::get_property(const nlohmann::json &json, const std::initializer_list<std::string> &keys)
{
	for (auto &key : keys)
	{
		if (json.contains(key))
			return json.at(key);
	}
	return nlohmann::json::object();
}
