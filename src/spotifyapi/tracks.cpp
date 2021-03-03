#include "spotifyapi.hpp"

// Currently unavailable:
// tracks
// audio-features
// audio-analysis/{id}

void Spotify::getTrack(const QString &id,
	const std::function<void(const Track &track)> &callback)
{
	get(QString("tracks/%1").arg(id), [callback](const QJsonDocument &json)
	{
		callback(Track(json.object()));
	});
}

void Spotify::trackAudioFeatures(const QString &trackId,
	const std::function<void(const AudioFeatures &features)> &callback)
{
	get(QString("audio-features/%1")
		.arg(trackId.startsWith("spotify:track:")
			? trackId.right(trackId.length() - QString("spotify:track:").length())
			: trackId), [callback](const QJsonDocument &json)
	{
		callback(AudioFeatures(json.object()));
	});
}
