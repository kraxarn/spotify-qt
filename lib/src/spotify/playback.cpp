#include "lib/spotify/playback.hpp"

void lib::spt::from_json(const nlohmann::json &j, playback &p)
{
	if (!j.is_object() || !j.contains("item"))
	{
		return;
	}

	j.at("progress_ms").get_to(p.progress_ms);
	j.at("item").get_to(p.item);
	j.at("is_playing").get_to(p.is_playing);
	j.at("shuffle_state").get_to(p.shuffle);
	j.at("context").get_to(p.context);
	j.at("device").get_to(p.device);

	auto repeat_state = j.at("repeat_state").get<std::string>();
	p.repeat = repeat_state == "track"
		? lib::repeat_state::track
		: repeat_state == "context"
			? lib::repeat_state::context
			: lib::repeat_state::off;
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

auto lib::spt::playback::metadata() const -> nlohmann::json
{
	auto artist_names = entity::combine_names(item.artists);

	return {
		{"xesam:title", item.name},
		{"xesam:artist", artist_names},
		{"xesam:album", item.album},
		{"xesam:albumArtist", artist_names},
		{"xesam:url", lib::fmt::format("https://open.spotify.com/track/{}", item.id)},
		{"mpris:length", item.duration * 1000},
		{"mpris:artUrl", item.image},
		{"mpris:trackid", lib::fmt::format("spotify:track:{}", item.id)},
	};
}

auto lib::spt::playback::volume() const -> int
{
	return device.volume_percent;
}
