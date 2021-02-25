#include "spotifyapi.hpp"

// Currently unavailable:
// tracks
// audio-features
// audio-analysis/{id}

spt::Track Spotify::getTrack(const QString &id)
{
	return spt::Track(getAsObject(QString("tracks/%1").arg(id)));
}

AudioFeatures Spotify::trackAudioFeatures(QString trackId)
{
	auto json = getAsObject(QString("audio-features/%1")
		.arg(trackId.startsWith("spotify:track:")
			? trackId.remove(0, QString("spotify:track:").length())
			: trackId));
	return AudioFeatures(json);
}
