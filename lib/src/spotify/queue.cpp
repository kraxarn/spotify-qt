#include "lib/spotify/queue.hpp"

void lib::spt::from_json(const nlohmann::json &json, lib::spt::queue &queue)
{
	if (!json.is_object())
	{
		return;
	}

	const auto &currently_playing = json.at("currently_playing");
	if (!currently_playing.is_null())
	{
		currently_playing.get_to(queue.currently_playing);
	}

	json.at("queue").get_to(queue.tracks);
}
