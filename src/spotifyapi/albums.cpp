#include "spotifyapi.hpp"

void Spotify::albumTracks(const QString &albumId,
	const std::function<void(const std::vector<Track> &tracks)> &callback)
{
	get(QString("albums/%1").arg(albumId),
		[this, albumId, callback](const QJsonDocument &json)
		{
			auto albumName = json["name"].toString();
			std::vector<Track> tracks;

			// Loop through all items
			for (auto track : json["tracks"].toObject()["items"].toArray())
			{
				auto t = Track(track.toObject());
				// Album name is not included, so we have to set it manually
				t.album = albumName;
				tracks.push_back(t);
			}

			// Check if we have any more to add
			auto tracksLimit = json["tracks"].toObject()["limit"].toInt();
			if (json["tracks"].toObject()["total"].toInt() > tracksLimit)
			{
				this->albumTracks(albumId, [tracks, callback](const std::vector<Track> &result)
				{
					callback(lib::vector::combine(tracks, result));
				});
			}

			// Return final vector
			callback(tracks);
		});
}

void Spotify::getAlbum(const QString &id,
	const std::function<void(const Album &album)> &callback)
{
	get(QString("albums/%1").arg(id), [callback](const QJsonDocument &json)
	{
		callback(Album(json.object()));
	});
}

void Spotify::albumTracks(const QString &albumId, const QString &albumName, int offset,
	const std::function<void(const std::vector<Track> &tracks)> &callback)
{
	get(QString("albums/%1/tracks?limit=50&offset=%2")
			.arg(albumId).arg(offset),
		[this, callback, albumId, albumName](const QJsonDocument &json)
		{
			auto trackItems = json["items"].toArray();
			std::vector<Track> tracks;

			// Add all in current page
			for (auto item : trackItems)
			{
				auto t = Track(item.toObject());
				// Album name is not included, so we have to set it manually
				t.album = albumName;
				tracks.push_back(t);
			}

			// Add all in next page
			if (json.object().contains("next") && !json["next"].isNull())
			{
				this->albumTracks(albumId, albumName,
					json["offset"].toInt() + json["limit"].toInt(),
					[tracks, callback](const std::vector<Track> &result)
					{
						callback(lib::vector::combine(tracks, result));
					});
			}

			callback(tracks);
		});
}
