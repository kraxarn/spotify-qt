#include "spotifyapi.hpp"

QVector<Track> Spotify::albumTracks(const QString &albumID)
{
	auto json = getAsObject(QString("albums/%1").arg(albumID));
	auto albumName = json["name"].toString();
	QVector<Track> tracks;
	tracks.reserve(json["total_tracks"].toInt());

	// Loop through all items
	for (auto track : json["tracks"].toObject()["items"].toArray())
	{
		auto t = Track(track.toObject());
		// Album name is not included, so we have to set it manually
		t.album = albumName;
		tracks.append(t);
	}

	// Check if we have any more to add
	auto tracksLimit = json["tracks"].toObject()["limit"].toInt();
	if (json["tracks"].toObject()["total"].toInt() > tracksLimit)
		tracks.append(albumTracks(albumID, albumName, tracksLimit));

	// Return final vector
	return tracks;
}

spt::Album Spotify::getAlbum(const QString &id)
{
	return spt::Album(getAsObject(QString("albums/%1").arg(id)));
}

QVector<Track> Spotify::albumTracks(const QString &albumId, const QString &albumName, int offset)
{
	auto json = getAsObject(QString("albums/%1/tracks?limit=50&offset=%2")
		.arg(albumId).arg(offset));
	auto trackItems = json["items"].toArray();
	QVector<Track> tracks;
	tracks.reserve(50);

	// Add all in current page
	for (auto item : trackItems)
	{
		auto t = Track(item.toObject());
		// Album name is not included, so we have to set it manually
		t.album = albumName;
		tracks.append(t);
	}

	// Add all in next page
	if (json.contains("next") && !json["next"].isNull())
		tracks.append(albumTracks(albumId, albumName,
			json["offset"].toInt() + json["limit"].toInt()));
	return tracks;
}
