#include "lib/spotify/playback.hpp"

void lib::spt::from_json(const nlohmann::json &j, playback &p)
{
	if (!j.is_object() || j.at("item").is_null())
		return;

	j.at("progress_ms").get_to(p.progress_ms);
	j.at("item").get_to(p.item);
	j.at("is_playing").get_to(p.is_playing);
	j.at("repeat_state").get_to(p.repeat);
	j.at("shuffle_state").get_to(p.repeat);
	j.at("context").get_to(p.context);
	j.at("device").get_to(p.device);
}

void lib::spt::to_json(nlohmann::json &j, const playback &p)
{
	j = nlohmann::json{
		{"progress_ms", p.progress_ms},
		{"item", p.item},
		{"is_playing", p.is_playing},
		{"repeat", p.repeat},
		{"shuffle", p.shuffle}
	};
}

nlohmann::json lib::spt::playback::metadata() const
{
	return {
		{"xesam:title", item.name},
		{"xesam:artist", item.artist},
		{"xesam:album", item.album},
		{"xesam:albumArtist", item.artist},
		{"xesam:url", lib::fmt::format("https://open.spotify.com/track/{}", item.id)},
		{"mpris:length", item.duration * 1000},
		{"mpris:artUrl", item.image},
		{"mpris:trackid", lib::fmt::format("spotify:track:{}", item.id)},
	};
}

int lib::spt::playback::volume() const
{
	return device.volume_percent;
}