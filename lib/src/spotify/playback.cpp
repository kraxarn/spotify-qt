#include "lib/spotify/playback.hpp"
#include "lib/enums.hpp"

#include "lib/fmt.hpp"

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

	if (j.contains("actions"))
	{
		const auto &actions = j.at("actions");
		if (actions.contains("disallows"))
		{
			const auto &disallows = actions.at("disallows");
			for (const auto &action: disallows.items())
			{
				const auto player_action = lib::enums<lib::player_action>::parse(action.key());
				p.disallowed_actions.insert(player_action);
			}
		}
	}
}

void lib::spt::to_json(nlohmann::json &j, const playback &p)
{
	std::vector<std::string> disallowed_action_strings;
	disallowed_action_strings.reserve(p.disallowed_actions.size());

	for (const auto &action: p.disallowed_actions)
	{
		const auto action_string = lib::enums<lib::player_action>::to_string(action);
		disallowed_action_strings.push_back(action_string);
	}

	j = nlohmann::json{
		{"progress_ms", p.progress_ms},
		{"item", p.item},
		{"is_playing", p.is_playing},
		{"repeat", p.repeat},
		{"shuffle", p.shuffle},
		{"disallowed_actions", disallowed_action_strings},
	};
}

auto lib::spt::playback::metadata() const -> nlohmann::json
{
	auto artist_names = entity::combine_names(item.artists);

	return {
		{"xesam:title", item.name},
		{"xesam:artist", artist_names},
		{"xesam:album", item.album.name},
		{"xesam:albumArtist", artist_names},
		{"xesam:url", lib::fmt::format("https://open.spotify.com/track/{}", item.id)},
		{"mpris:length", item.duration * 1000},
		{"mpris:artUrl", item.image_large()},
		{"mpris:trackid", lib::fmt::format("spotify:track:{}", item.id)},
	};
}

auto lib::spt::playback::volume() const -> int
{
	return device.volume_percent;
}

auto lib::spt::playback::is_allowed(const lib::player_action &action) const -> bool
{
	return disallowed_actions.find(action) == disallowed_actions.end();
}
