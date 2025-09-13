#include "lib/lyrics/error.hpp"

void lib::lrc::from_json(const nlohmann::json &json, error &error)
{
	if (!json.is_object())
	{
		return;
	}

	json.at("message").get_to(error.message);
	json.at("name").get_to(error.name);
	json.at("statusCode").get_to(error.status_code);
}
