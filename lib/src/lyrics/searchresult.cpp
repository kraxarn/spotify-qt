#include "lib/lyrics/searchresult.hpp"

void lib::lrc::from_json(const nlohmann::json &json, lib::lrc::search_result &result)
{
	if (!json.is_object())
	{
		return;
	}

	json.at("id").get_to(result.lyrics_id);
	json.at("name").get_to(result.track);

	if (json.contains("artists"))
	{
		for (const auto &artist: json.at("artists"))
		{
			result.artists.push_back(artist.at("name"));
		}
	}

	if (json.contains("album"))
	{
		json.at("album").at("name").get_to(result.album);
	}
}
