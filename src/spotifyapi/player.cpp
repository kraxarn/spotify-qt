#include "spotifyapi.hpp"

// Currently unavailable:
// me/player/currently-playing

void Spotify::currentPlayback(callback<spt::Playback> &callback)
{
	get("me/player", [callback](const QJsonObject &json)
	{
		callback(Playback(json));
	});
}

void Spotify::setDevice(const QString &deviceId, callback<QString> &callback)
{
	QJsonDocument body({
		QPair<QString, QJsonArray>("device_ids", {
			deviceId
		}),
	});
	currentDevice = deviceId;
	put("me/player", body, callback);
}

void Spotify::devices(callback<std::vector<Device>> &callback)
{
	get("me/player/devices", [callback](const QJsonObject &json)
	{
		auto items = json["devices"].toArray();
		std::vector<Device> devices(items.size());
		for (auto i = 0; i < items.size(); i++)
			devices[i] = Device(items.at(i).toObject());
		callback(devices);
	});
}

void Spotify::playTracks(int trackIndex, const QString &context, callback<QString> &callback)
{
	lib::log::dev("Playing track {} from {}", trackIndex, context.toStdString());

	QJsonDocument body({
		QPair<QString, QString>("context_uri", context),
		QPair<QString, QJsonObject>("offset", QJsonObject({
			QPair<QString, int>("position", trackIndex)
		}))
	});

	put(currentDevice == nullptr || currentDevice.isEmpty()
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1")
			.arg(currentDevice), body, callback);
}

void Spotify::playTracks(int trackIndex, const QList<QString> &all, callback<QString> &callback)
{
	lib::log::dev("Playing track {} ({} total)", trackIndex, all.length());

	auto maxQueue = settings.spotify.max_queue;
	QStringList items = all;
	if (all.length() > maxQueue)
	{
		lib::log::warn("Attempting to queue {} tracks, but only {} allowed",
			all.length(), maxQueue);
		items = all.mid(trackIndex, maxQueue);
		trackIndex = 0;
	}

	QJsonDocument body({
		QPair<QString, QJsonArray>("uris", JsonUtils::toJsonArray(items)),
		QPair<QString, QJsonObject>("offset", QJsonObject({
			QPair<QString, int>("position", trackIndex)
		}))
	});

	put(currentDevice == nullptr || currentDevice.isEmpty()
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1")
			.arg(currentDevice), body, callback);
}

void Spotify::playTracks(const QString &context, callback<QString> &callback)
{
	lib::log::dev("Playing track from {}", context.toStdString());

	QJsonDocument body({
		QPair<QString, QString>("context_uri", context),
	});

	put(currentDevice == nullptr
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1")
			.arg(currentDevice), body, callback);
}

void Spotify::resume(callback<QString> &callback)
{
	put("me/player/play", callback);
}

void Spotify::pause(callback<QString> &callback)
{
	put("me/player/pause", callback);
}

void Spotify::next(callback<QString> &callback)
{
	post("me/player/next", callback);
}

void Spotify::previous(callback<QString> &callback)
{
	post("me/player/previous", callback);
}

void Spotify::seek(int position, callback<QString> &callback)
{
	put(QString("me/player/seek?position_ms=%1")
		.arg(position), callback);
}

void Spotify::setRepeat(const QString &state, callback<QString> &callback)
{
	put(QString("me/player/repeat?state=%1")
		.arg(state), callback);
}

void Spotify::setVolume(int volume, callback<QString> &callback)
{
	put(QString("me/player/volume?volume_percent=%1")
		.arg(volume), callback);
}

void Spotify::setShuffle(bool enabled, callback<QString> &callback)
{
	put(QString("me/player/shuffle?state=%1")
		.arg(enabled ? "true" : "false"), callback);
}

QVector<Track> Spotify::recentlyPlayed()
{
	return loadItems<Track>("me/player/recently-played?limit=50");
}

void Spotify::addToQueue(const QString &uri, callback<QString> &callback)
{
	post(QString("me/player/queue?uri=%1").arg(uri),
		callback);
}
