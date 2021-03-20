#include "spotifyapi.hpp"

// Currently unavailable:
// me/player/currently-playing

void Spotify::currentPlayback(lib::callback<spt::Playback> &callback)
{
	get("me/player", [callback](const QJsonObject &json)
	{
		callback(Playback(json));
	});
}

void Spotify::setDevice(const std::string &deviceId, lib::callback<QString> &callback)
{
	current_device = deviceId;
	put("me/player", {
		{"device_ids", {
			deviceId
		}}
	}, callback);
}

void Spotify::setDevice(const lib::spt::device &device, lib::callback<QString> &callback)
{
	setDevice(device.id, callback);
}

void Spotify::devices(lib::callback<std::vector<lib::spt::device>> &callback)
{
	get("me/player/devices", [callback](const nlohmann::json &json)
	{
		callback(json.at("devices").get<std::vector<lib::spt::device>>());
	});
}

void Spotify::playTracks(int trackIndex, const QString &context, lib::callback<QString> &callback)
{
	lib::log::dev("Playing track {} from {}", trackIndex, context.toStdString());

	nlohmann::json body = {
		{"context_uri", context.toStdString()},
		{"offset", {
			{"position", trackIndex}
		}}
	};

	put(QString::fromStdString(current_device.empty()
			? "me/player/play"
			: lib::fmt::format("me/player/play?device_id={}", current_device)),
		body, callback);
}

void Spotify::playTracks(int trackIndex, const std::vector<std::string> &all,
	lib::callback<QString> &callback)
{
	lib::log::dev("Playing track {} ({} total)", trackIndex, all.size());

	auto maxQueue = settings.spotify.max_queue;
	std::vector<std::string> items = all;
	if (all.size() > maxQueue)
	{
		lib::log::warn("Attempting to queue {} tracks, but only {} allowed",
			all.size(), maxQueue);
		items = lib::vector::sub(all, trackIndex, maxQueue);
		trackIndex = 0;
	}

	nlohmann::json body = {
		{"uris", items},
		{"offset", {
			{"position", trackIndex}
		}}
	};

	put(QString::fromStdString(current_device.empty()
			? "me/player/play"
			: lib::fmt::format("me/player/play?device_id={}", current_device)),
		body, callback);
}

void Spotify::playTracks(const QString &context, lib::callback<QString> &callback)
{
	lib::log::dev("Playing track from {}", context.toStdString());

	nlohmann::json body = {
		{"context_uri", context.toStdString()}
	};

	put(QString::fromStdString(current_device.empty()
			? "me/player/play"
			: lib::fmt::format("me/player/play?device_id={}", current_device)),
		body, callback);
}

void Spotify::resume(lib::callback<QString> &callback)
{
	put("me/player/play", callback);
}

void Spotify::pause(lib::callback<QString> &callback)
{
	put("me/player/pause", callback);
}

void Spotify::next(lib::callback<QString> &callback)
{
	post("me/player/next", callback);
}

void Spotify::previous(lib::callback<QString> &callback)
{
	post("me/player/previous", callback);
}

void Spotify::seek(int position, lib::callback<QString> &callback)
{
	put(QString("me/player/seek?position_ms=%1")
		.arg(position), callback);
}

void Spotify::setRepeat(const QString &state, lib::callback<QString> &callback)
{
	put(QString("me/player/repeat?state=%1")
		.arg(state), callback);
}

void Spotify::setVolume(int volume, lib::callback<QString> &callback)
{
	put(QString("me/player/volume?volume_percent=%1")
		.arg(volume), callback);
}

void Spotify::setShuffle(bool enabled, lib::callback<QString> &callback)
{
	put(QString("me/player/shuffle?state=%1")
		.arg(enabled ? "true" : "false"), callback);
}

QVector<Track> Spotify::recentlyPlayed()
{
	return loadItems<Track>("me/player/recently-played?limit=50");
}

void Spotify::addToQueue(const QString &uri, lib::callback<QString> &callback)
{
	post(QString("me/player/queue?uri=%1").arg(uri),
		callback);
}
