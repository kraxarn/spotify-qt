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

QString Spotify::playTracks(int trackIndex, const QString &context)
{
	lib::log::dev("Playing track {} from {}", trackIndex, context.toStdString());

	QVariantMap body;
	body["context_uri"] = context;
	body["offset"] = QJsonObject({
		QPair<QString, int>("position", trackIndex)
	});

	return put(currentDevice == nullptr || currentDevice.isEmpty()
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1").arg(currentDevice), &body);
}

QString Spotify::playTracks(int trackIndex, const QList<QString> &all)
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

	return put(currentDevice == nullptr || currentDevice.isEmpty()
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1").arg(currentDevice), &body);
}

QString Spotify::playTracks(const QString &context)
{
	lib::log::dev("Playing track from {}", context.toStdString());

	QVariantMap body;
	body["context_uri"] = context;

	return put(currentDevice == nullptr
		? QString("me/player/play")
		: QString("me/player/play?device_id=%1").arg(currentDevice), &body);
}

QString Spotify::resume()
{
	return put("me/player/play");
}

QString Spotify::pause()
{
	return put("me/player/pause");
}

QString Spotify::next()
{
	return post("me/player/next");
}

QString Spotify::previous()
{
	return post("me/player/previous");
}

QString Spotify::seek(int position)
{
	return put(QString("me/player/seek?position_ms=%1").arg(position));
}

QString Spotify::setRepeat(const QString &state)
{
	return put(QString("me/player/repeat?state=%1").arg(state));
}

QString Spotify::setVolume(int volume)
{
	return put(QString("me/player/volume?volume_percent=%1").arg(volume));
}

QString Spotify::setShuffle(bool enabled)
{
	return put(QString("me/player/shuffle?state=%1")
		.arg(enabled ? "true" : "false"));
}

QVector<Track> Spotify::recentlyPlayed()
{
	return loadItems<Track>("me/player/recently-played?limit=50");
}

QString Spotify::addToQueue(const QString &uri)
{
	return post(QString("me/player/queue?uri=%1").arg(uri));
}
