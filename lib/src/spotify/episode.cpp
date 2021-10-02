#include "lib/spotify/episode.hpp"

void lib::spt::from_json(const nlohmann::json &j, episode &e)
{
	if (!j.is_object())
	{
		return;
	}

	j.at("audio_preview_url").get_to(e.audio_preview_url);
	j.at("description").get_to(e.description);
	j.at("duration_ms").get_to(e.duration_ms);
	j.at("explicit").get_to(e.is_explicit);
	j.at("external_urls").get_to(e.external_urls);
	j.at("href").get_to(e.href);
	j.at("html_description").get_to(e.html_description);
	j.at("id").get_to(e.id);
	j.at("is_externally_hosted").get_to(e.is_externally_hosted);
	j.at("is_playable").get_to(e.is_playable);
	j.at("languages").get_to(e.languages);
	j.at("name").get_to(e.name);
	j.at("release_date").get_to(e.release_date);
	j.at("release_date_precision").get_to(e.release_date_precision);
	j.at("uri").get_to(e.uri);

	lib::json::find_item("image", j, e.image);
}

auto lib::spt::episode::to_track(const lib::spt::show &show) const -> lib::spt::track
{
	lib::spt::track track;

	track.id = id;
	track.album.name = show.name;
	track.artists.emplace_back(std::string(), show.publisher);
	track.name = name;
	track.image = show.image;
	track.duration = duration_ms;
	track.is_local = false;
	track.is_playable = is_playable;
	track.added_at = release_date;

	return track;
}
