#include "spotifyapi.hpp"

// Some calls are currently only available from the Playlist class

// Currently unavailable:
// users/{user_id}/playlists
// users/{user_id}/playlists
// playlists/{playlist_id}/tracks
// playlists/{playlist_id}/images

void Spotify::playlists(int offset,
	const std::function<void(const std::vector<Playlist> &playlists)> &callback)
{
	// Request playlists
	get(QString("me/playlists?limit=50&offset=%1").arg(offset),
		[this, callback](const QJsonDocument &json)
		{
			// Parse as playlists
			auto items = json["items"].toArray();

			// Create list of playlists
			auto size = json["total"].toInt();
			std::vector<Playlist> playlists;

			// Loop through all items
			for (auto item : items)
				playlists.emplace_back(item.toObject());

			// Paging
			if (json.object().contains("next") && !json["next"].isNull())
			{
				this->playlists(json["offset"].toInt() + json["limit"].toInt(),
					[callback, playlists](const std::vector<Playlist> &result)
					{
						callback(lib::vector::combine(playlists, result));
					});
			}
		});
}

void Spotify::playlists(const std::function<void(const std::vector<Playlist> &playlists)> &callback)
{
	playlists(0, callback);
}

void Spotify::playlist(const QString &playlistId,
	const std::function<void(const Playlist &playlist)> &callback)
{
	get(QString("playlists/%1").arg(playlistId),
		[callback](const QJsonDocument &json)
		{
			callback(Playlist(json.object()));
		});
}

void Spotify::editPlaylist(const Playlist &playlist,
	const std::function<void(const QString &result)> &callback)
{
	QVariantMap body({
		{"name", playlist.name},
		{"public", playlist.isPublic},
		{"collaborative", playlist.collaborative},
		{"description", playlist.description}
	});

	put(QString("playlists/%1").arg(playlist.id), &body, callback);
}

void Spotify::addToPlaylist(const QString &playlistId, const QString &trackId,
	const std::function<void(const QString &result)> &callback)
{
	post(QString("playlists/%1/tracks?uris=%2")
		.arg(playlistId).arg(trackId), callback);
}

void Spotify::removeFromPlaylist(const QString &playlistId, const QString &trackId, int pos,
	const std::function<void(const QString &result)> &callback)
{
	QJsonDocument json({
		QPair<QString, QJsonArray>("tracks", QJsonArray({
			QJsonObject({
				QPair<QString, QString>("uri", trackId),
				QPair<QString, QJsonArray>("positions", QJsonArray({
					pos
				}))
			})
		}))
	});

	del(QString("playlists/%1/tracks").arg(playlistId), json, callback);
}
