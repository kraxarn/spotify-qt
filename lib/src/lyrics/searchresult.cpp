#include "lib/lyrics/searchresult.hpp"

void lib::lrc::from_json(const nlohmann::json &json, lib::lrc::search_result &result)
{
	if (!json.is_object())
	{
		return;
	}

	json.at("id").get_to(result.lyrics_id);
}
