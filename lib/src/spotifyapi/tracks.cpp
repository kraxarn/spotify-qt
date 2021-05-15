#include "lib/spotify/api.hpp"

using namespace lib::spt;

// Currently unavailable:
// tracks
// audio-features
// audio-analysis/{id}

void api::track(const std::string &id,
	lib::callback<lib::spt::track> &callback)
{
	get(lib::fmt::format("tracks/{}", id), callback);
}

void api::track_audio_features(const std::string &track_id,
	lib::callback<lib::spt::audio_features> &callback)
{
	get(lib::fmt::format("audio-features/{}", api::to_id(track_id)), callback);
}
