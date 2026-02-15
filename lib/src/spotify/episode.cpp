#include "lib/spotify/episode.hpp"

void lib::spt::from_json(const nlohmann::json &j, episode &e)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("duration_ms").get_to(e.duration_ms);
	j.at("id").get_to(e.id);
	j.at("images").get_to(e.images);
	j.at("is_playable").get_to(e.is_playable);
	j.at("name").get_to(e.name);
	j.at("release_date").get_to(e.release_date);
}

auto lib::spt::episode::to_track(const lib::spt::show &show) const -> lib::spt::track
{
	lib::spt::track track;

	track.id = id;
	track.album.name = show.name;
	track.name = name;
	track.images = show.images;
	track.duration = duration_ms;
	track.is_local = false;
	track.is_playable = is_playable;
	track.added_at = release_date;

	track.artists.emplace_back(std::string(), show.publisher.empty()
		? std::string("Unknown")
		: show.publisher);

	return track;
}
