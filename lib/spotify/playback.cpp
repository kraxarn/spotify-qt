#include "playback.hpp"

using namespace lib::spt;

Playback::Playback(const nlohmann::json &json)
{
	if (json.at("item").is_null())
		return;

	json.at("progress_ms").get_to(progressMs);
	item = track(json.at("item"));
	json.at("is_playing").get_to(isPlaying);
	json.at("repeat_state").get_to(repeat);
	json.at("shuffle_state").get_to(shuffle);
	if (json.at("context").is_object())
	{
		auto context = json.at("context");
		context.at("uri").get_to(contextUri);
		context.at("type").get_to(contextType);
	}
	device = Device(json.at("device"));
}

nlohmann::json Playback::metadata() const
{
	std::string itemName(isPlaying ? item.name : "");
	std::vector<std::string> itemArtist({
		isPlaying ? item.artist : ""
	});
	std::string itemAlbum(isPlaying ? item.album : "");
	std::string itemId(isPlaying ? item.id : "");
	auto itemDuration = isPlaying ? item.duration : 0;
	std::string itemImage(isPlaying ? item.image : "");

	return {
		{"xesam:title", itemName},
		{"xesam:artist", itemArtist},
		{"xesam:album", itemAlbum},
		{"xesam:albumArtist", itemArtist},
		{"xesam:url", fmt::format("https://open.spotify.com/track/{}", itemId)},
		{"mpris:length", itemDuration * 1000},
		{"mpris:artUrl", itemImage},
		{"mpris:trackid", fmt::format("spotify:track:{}", itemId)},
	};
}

nlohmann::json Playback::toJson() const
{
	return {
		{"progress_ms", progressMs},
		{"item", item.to_json()},
		{"is_playing", isPlaying},
		{"repeat", repeat},
		{"shuffle", shuffle}
	};
}

int Playback::volume() const
{
	return device.volumePercent;
}
