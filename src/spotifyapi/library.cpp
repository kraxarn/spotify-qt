#include "spotifyapi.hpp"

// Currently unavailable:
// me/albums
// me/albums/contains
// me/tracks/contains
// me/shows
// me/shows/contains

std::vector<lib::spt::album> Spotify::savedAlbums()
{
	auto json = getAsJson(std::string("me/albums"));
	auto albumItems = json["items"];
	std::vector<lib::spt::album> albums;
	albums.reserve(10);
	for (auto &item : albumItems)
		albums.push_back(item.get<lib::spt::album>());
	return albums;
}

std::vector<lib::spt::track> Spotify::savedTracks(int offset)
{
	auto json = getAsJson(QString("me/tracks?limit=50&offset=%1").arg(offset));
	auto trackItems = json["items"].items();
	std::vector<lib::spt::track> tracks;

	// Add all in current page
	for (auto &item : trackItems)
		tracks.push_back(item.value().get<lib::spt::track>());

	// Add all in next page
	if (json.contains("next") && !json["next"].is_null())
	{
		lib::vector::append(tracks,
			savedTracks(json["offset"].get<int>() + json["limit"].get<int>()));
	}
	return tracks;
}

void Spotify::addSavedTrack(const std::string &trackId, lib::callback<QString> &callback)
{
	put("me/tracks", {
		{"ids", {
			lib::strings::starts_with(trackId, "spotify:track:")
				? trackId.substr(0, std::string("spotify:track").size())
				: trackId
		}}
	}, callback);
}

void Spotify::removeSavedTrack(const std::string &trackId, lib::callback<QString> &callback)
{
	del("me/tracks", {
		{"ids", {
			lib::strings::starts_with(trackId, "spotify:track:")
				? trackId.substr(0, std::string("spotify:track").size())
				: trackId
		}}
	}, callback);
}

void Spotify::isSavedTrack(const QStringList &trackIds,
	lib::callback<std::vector<bool>> &callback)
{
	get(QString("me/tracks/contains?ids=%1")
		.arg(trackIds.join(',')), [callback](const QJsonArray &json)
	{
		std::vector<bool> values;
		for (auto value : json)
			values.push_back(value.toBool());
		callback(values);
	});
}
