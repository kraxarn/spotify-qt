#include "spotifyapi.hpp"

// Currently unavailable:
// me/player/currently-playing

void Spotify::currentPlayback(const std::function<void(const spt::Playback &playback)> &callback)
{
	get("me/player", [callback](const QJsonDocument &json)
	{
		callback(Playback(json.object()));
	});
}

void Spotify::setDevice(const QString &deviceId,
	const std::function<void(const QString &status)> &callback)
{
	QVariantMap body;
	body["device_ids"] = QStringList({
		deviceId
	});
	currentDevice = deviceId;
	put("me/player", &body, callback);
}

void Spotify::devices(const std::function<void(const std::vector<Device> &devices)> &callback)
{
	get("me/player/devices", [callback](const QJsonDocument &json)
	{
		auto items = json["devices"].toArray();
		std::vector<Device> devices(items.size());
		for (auto i = 0; i < items.size(); i++)
			devices[i] = Device(items.at(i).toObject());
		callback(devices);
	});
}

void Spotify::playTracks(int trackIndex, const QString &context,
	const std::function<void(const QString &result)> &callback)
{
	lib::log::dev("Playing track {} from {}", trackIndex, context.toStdString());

	QVariantMap body;
	body["context_uri"] = context;
	body["offset"] = QJsonObject({
		QPair<QString, int>("position", trackIndex)
	});

	put(currentDevice == nullptr || currentDevice.isEmpty()
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1")
			.arg(currentDevice), &body, callback);
}

void Spotify::playTracks(int trackIndex, const QList<QString> &all,
	const std::function<void(const QString &result)> &callback)
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

	QVariantMap body;
	body["uris"] = items;
	body["offset"] = QJsonObject({
		QPair<QString, int>("position", trackIndex)
	});

	put(currentDevice == nullptr || currentDevice.isEmpty()
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1")
			.arg(currentDevice), &body, callback);
}

void Spotify::playTracks(const QString &context,
	const std::function<void(const QString &result)> &callback)
{
	lib::log::dev("Playing track from {}", context.toStdString());

	QVariantMap body;
	body["context_uri"] = context;

	put(currentDevice == nullptr
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1")
			.arg(currentDevice), &body, callback);
}

void Spotify::resume(const std::function<void(const QString &result)> &callback)
{
	put("me/player/play", callback);
}

void Spotify::pause(const std::function<void(const QString &result)> &callback)
{
	put("me/player/pause", callback);
}

void Spotify::next(const std::function<void(const QString &result)> &callback)
{
	post("me/player/next", callback);
}

void Spotify::previous(const std::function<void(const QString &result)> &callback)
{
	post("me/player/previous", callback);
}

void Spotify::seek(int position,
	const std::function<void(const QString &result)> &callback)
{
	put(QString("me/player/seek?position_ms=%1")
		.arg(position), callback);
}

void Spotify::setRepeat(const QString &state,
	const std::function<void(const QString &result)> &callback)
{
	put(QString("me/player/repeat?state=%1")
		.arg(state), callback);
}

void Spotify::setVolume(int volume,
	const std::function<void(const QString &result)> &callback)
{
	put(QString("me/player/volume?volume_percent=%1")
		.arg(volume), callback);
}

void Spotify::setShuffle(bool enabled,
	const std::function<void(const QString &result)> &callback)
{
	put(QString("me/player/shuffle?state=%1")
		.arg(enabled ? "true" : "false"), callback);
}

void Spotify::recentlyPlayed(const std::function<void(const std::vector<Track> &tracks)> &callback)
{
	get("me/player/recently-played?limit=50", callback);
}

void Spotify::addToQueue(const QString &uri,
	const std::function<void(const QString &result)> &callback)
{
	post(QString("me/player/queue?uri=%1")
		.arg(uri), callback);
}
