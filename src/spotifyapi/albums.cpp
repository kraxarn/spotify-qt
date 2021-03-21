#include "spotifyapi.hpp"

std::vector<lib::spt::track> Spotify::albumTracks(const std::string &albumId)
{
	auto json = getAsJson(QString("albums/%1").arg(QString::fromStdString(albumId)));
	auto albumName = json.at("name").get<std::string>();
	std::vector<lib::spt::track> tracks;

	// Loop through all items
	for (auto &track : json.at("tracks").at("items").items())
	{
		auto t = track.value().get<lib::spt::track>();
		// Album name is not included, so we have to set it manually
		t.album = albumName;
		tracks.push_back(t);
	}

	// Check if we have any more to add
	auto tracksLimit = json.at("tracks").at("limit").get<int>();
	if (json.at("tracks").at("total").get<int>() > tracksLimit)
		lib::vector::append(tracks, albumTracks(albumId, albumName, tracksLimit));

	// Return final vector
	return tracks;
}

spt::Album Spotify::getAlbum(const QString &id)
{
	return spt::Album(getAsObject(QString("albums/%1").arg(id)));
}

std::vector<lib::spt::track> Spotify::albumTracks(const std::string &albumId,
	const std::string &albumName, int offset)
{
	auto json = getAsJson(QString("albums/%1/tracks?limit=50&offset=%2")
		.arg(QString::fromStdString(albumId))
		.arg(offset));
	auto trackItems = json["items"].items();
	std::vector<lib::spt::track> tracks;

	// Add all in current page
	for (auto &item : trackItems)
	{
		auto t = item.value().get<lib::spt::track>();
		// Album name is not included, so we have to set it manually
		t.album = albumName;
		tracks.push_back(t);
	}

	// Add all in next page
	if (json.contains("next") && !json.at("next").is_null())
	{
		lib::vector::append(tracks, albumTracks(albumId, albumName,
			json["offset"].get<int>() + json["limit"].get<int>()));
	}
	return tracks;
}
