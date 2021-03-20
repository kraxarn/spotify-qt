#include "spotifyapi.hpp"

// Currently unavailable:
// me/albums
// me/albums/contains
// me/tracks/contains
// me/shows
// me/shows/contains

QVector<Album> Spotify::savedAlbums()
{
	auto json = getAsObject("me/albums");
	auto albumItems = json["items"].toArray();
	QVector<Album> albums;
	albums.reserve(10);
	for (auto item : albumItems)
		albums.append(Album(item.toObject()["album"].toObject()));
	return albums;
}

QVector<Track> Spotify::savedTracks(int offset)
{
	auto json = getAsObject(QString("me/tracks?limit=50&offset=%1").arg(offset));
	auto trackItems = json["items"].toArray();
	QVector<Track> tracks;
	tracks.reserve(50);

	// Add all in current page
	for (auto item : trackItems)
		tracks.append(Track(item.toObject()));

	// Add all in next page
	if (json.contains("next") && !json["next"].isNull())
		tracks.append(savedTracks(json["offset"].toInt() + json["limit"].toInt()));
	return tracks;
}

QString Spotify::addSavedTrack(const QString &trackId)
{
	QVariantMap body;
	body["ids"] = QStringList({
		trackId.startsWith("spotify:track")
			? trackId.right(trackId.length() - QString("spotify:track:").length())
			: trackId
	});
	return put("me/tracks", &body);
}

QString Spotify::removeSavedTrack(const QString &trackId)
{
	QJsonDocument json({
		QPair<QString, QJsonArray>("ids", QJsonArray({
			trackId.startsWith("spotify:track")
				? trackId.right(trackId.length() - QString("spotify:track:").length())
				: trackId
		}))
	});

	return del("me/tracks", json);
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
