#include "spotifyapi.hpp"

// Currently unavailable:
// tracks
// audio-features
// audio-analysis/{id}

lib::spt::track Spotify::getTrack(const std::string &id)
{
	return getAsJson(QString("tracks/%1").arg(QString::fromStdString(id)))
		.get<lib::spt::track>();
}

AudioFeatures Spotify::trackAudioFeatures(const std::string &trackId)
{
	auto json = getAsObject(QString::fromStdString(lib::fmt::format("audio-features/{}",
		lib::strings::starts_with(trackId, "spotify:track:")
			? trackId.substr(0, std::string("spotify:track:").length())
			: trackId)));
	return AudioFeatures(json);
}
