#include "spotifyapi.hpp"

// Currently unavailable:
// me/albums
// me/albums/contains
// me/tracks/contains
// me/shows
// me/shows/contains

void Spotify::savedAlbums(const std::function<void(const std::vector<Album> &albums)> &callback)
{
	get("me/albums?limit=50", [callback](const QJsonDocument &json)
	{
		auto albumItems = json["items"].toArray();
		std::vector<Album> albums;
		for (auto item : albumItems)
			albums.emplace_back(item.toObject()["album"].toObject());
		callback(albums);
	});
}

void Spotify::savedTracks(int offset,
	const std::function<void(const std::vector<Track> &albums)> &callback)
{
	get(QString("me/tracks?limit=50&offset=%1").arg(offset),
		[this, callback](const QJsonDocument &json)
		{
			auto trackItems = json["items"].toArray();
			std::vector<Track> tracks;
			tracks.reserve(50);

			// Add all in current page
			for (auto item : trackItems)
				tracks.emplace_back(item.toObject());

			// Add all in next page
			if (json.object().contains("next") && !json["next"].isNull())
			{
				this->savedTracks(json["offset"].toInt() + json["limit"].toInt(),
					[tracks, callback](const std::vector<Track> &result)
					{
						callback(lib::vector::combine(tracks, result));
					});
			}
		});
}

void Spotify::savedTracks(const std::function<void(const std::vector<Track> &)> &callback)
{
	savedTracks(0, callback);
}

void Spotify::addSavedTrack(const QString &trackId,
	const std::function<void(const QString &result)> &callback)
{
	QVariantMap body;
	body["ids"] = QStringList({
		trackId.startsWith("spotify:track")
			? trackId.right(trackId.length() - QString("spotify:track:").length())
			: trackId
	});

	put("me/tracks", &body, callback);
}

void Spotify::removeSavedTrack(const QString &trackId,
	const std::function<void(const QString &result)> &callback)
{
	QJsonDocument json({
		QPair<QString, QJsonArray>("ids", QJsonArray({
			trackId.startsWith("spotify:track")
				? trackId.right(trackId.length() - QString("spotify:track:").length())
				: trackId
		}))
	});

	del("me/tracks", json, callback);
}
