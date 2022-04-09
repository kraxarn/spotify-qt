#include "lib/spotify/api.hpp"

// Currently unavailable:
// tracks
// audio-analysis/{id}

void lib::spt::api::track(const std::string &track_id,
	lib::callback<lib::spt::track> &callback)
{
	get(lib::fmt::format("tracks/{}", track_id), callback);
}

void lib::spt::api::track_audio_features(const std::string &track_id,
	lib::callback<lib::spt::audio_features> &callback)
{
	get(lib::fmt::format("audio-features/{}",
		lib::spt::api::to_id(track_id)), callback);
}

void lib::spt::api::track_audio_features(const std::vector<std::string> &track_ids,
	lib::callback<std::vector<lib::spt::audio_features>> &callback)
{
	get(lib::fmt::format("audio-features?ids={}",
			lib::strings::join(track_ids, ",")),
		[callback](const nlohmann::json &json)
		{
			callback(json.at("audio_features"));
		});
}
