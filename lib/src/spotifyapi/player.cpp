#include "lib/spotify/spotifyapi.hpp"

using namespace lib::spt;

// Currently unavailable:
// me/player/currently-playing

void api::current_playback(lib::callback<lib::spt::playback> &callback)
{
	get("me/player", [callback](const nlohmann::json &json)
	{
		callback(json);
	});
}

//region set_device

void api::set_device(const std::string &device_id, lib::callback<std::string> &callback)
{
	current_device = device_id;
	put("me/player", {
		{"device_ids", {
			device_id
		}}
	}, callback);
}

void api::set_device(const device &device, lib::callback<std::string> &callback)
{
	set_device(device.id, callback);
}

//endregion

void api::devices(lib::callback<std::vector<lib::spt::device>> &callback)
{
	get("me/player/devices", [callback](const nlohmann::json &json)
	{
		callback(json.at("devices").get<std::vector<lib::spt::device>>());
	});
}

//region play_tracks

void api::play_tracks(int trackIndex, const std::string &context,
	lib::callback<std::string> &callback)
{
	lib::log::dev("Playing track {} from {}", trackIndex, context);

	nlohmann::json body = {
		{"context_uri", context},
		{"offset", {
			{"position", trackIndex}
		}}
	};

	put(current_device.empty()
			? "me/player/play"
			: lib::fmt::format("me/player/play?device_id={}", current_device),
		body, callback);
}

void api::play_tracks(int track_index, const std::vector<std::string> &all,
	lib::callback<std::string> &callback)
{
	lib::log::dev("Playing track {} ({} total)", track_index, all.size());

	auto maxQueue = settings.spotify.max_queue;
	std::vector<std::string> items = all;
	if (all.size() > maxQueue)
	{
		lib::log::warn("Attempting to queue {} tracks, but only {} allowed",
			all.size(), maxQueue);
		items = lib::vector::sub(all, track_index, maxQueue);
		track_index = 0;
	}

	nlohmann::json body = {
		{"uris", items},
		{"offset", {
			{"position", track_index}
		}}
	};

	put(current_device.empty()
			? "me/player/play"
			: lib::fmt::format("me/player/play?device_id={}", current_device),
		body, callback);
}

void api::play_tracks(int track_index, const std::initializer_list<std::string> &all,
	lib::callback<std::string> &callback)
{
	play_tracks(track_index, std::vector<std::string>(all), callback);
}

void api::play_tracks(const std::string &context, lib::callback<std::string> &callback)
{
	lib::log::dev("Playing track from {}", context);

	nlohmann::json body = {
		{"context_uri", context}
	};

	put(current_device.empty()
			? "me/player/play"
			: lib::fmt::format("me/player/play?device_id={}", current_device),
		body, callback);
}

//endregion

void api::resume(lib::callback<std::string> &callback)
{
	put("me/player/play", callback);
}

void api::pause(lib::callback<std::string> &callback)
{
	put("me/player/pause", callback);
}

void api::next(lib::callback<std::string> &callback)
{
	post("me/player/next", callback);
}

void api::previous(lib::callback<std::string> &callback)
{
	post("me/player/previous", callback);
}

void api::seek(int position, lib::callback<std::string> &callback)
{
	put(lib::fmt::format("me/player/seek?position_ms={}", position), callback);
}

void api::set_repeat(lib::repeat_state state, lib::callback<std::string> &callback)
{
	std::string repeat;
	switch (state)
	{
		case repeat_state::track:
			repeat = "track";
			break;

		case repeat_state::context:
			repeat = "context";
			break;

		default:
			repeat = "off";
			break;
	}

	put(lib::fmt::format("me/player/repeat?state={}", repeat), callback);
}

void api::set_volume(int volume, lib::callback<std::string> &callback)
{
	put(lib::fmt::format("me/player/volume?volume_percent={}", volume), callback);
}

void api::set_shuffle(bool enabled, lib::callback<std::string> &callback)
{
	put(lib::fmt::format("me/player/shuffle?state={}", enabled), callback);
}

void api::recently_played(lib::callback<std::vector<lib::spt::track>> &callback)
{
	get_items("me/player/recently-played?limit=50", callback);
}

void api::add_to_queue(const std::string &uri, lib::callback<std::string> &callback)
{
	post(lib::fmt::format("me/player/queue?uri={}", uri), callback);
}