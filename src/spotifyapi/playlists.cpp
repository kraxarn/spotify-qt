#include "spotifyapi.hpp"

// Some calls are currently only available from the Playlist class

// Currently unavailable:
// users/{user_id}/playlists
// users/{user_id}/playlists
// playlists/{playlist_id}/tracks
// playlists/{playlist_id}/images

QVector<Playlist> Spotify::playlists(int offset)
{
	// Request playlists
	auto json = getAsObject(QString("me/playlists?limit=50&offset=%1").arg(offset));

	// Parse as playlists
	auto items = json["items"].toArray();

	// Create list of playlists
	auto size = json["total"].toInt();
	QVector<Playlist> playlists;
	playlists.reserve(size);

	// Loop through all items
	for (int i = 0; i < items.size(); i++)
		playlists.insert(i, Playlist(items.at(i).toObject()));

	// Paging
	if (json.contains("next") && !json["next"].isNull())
		playlists.append(this->playlists(json["offset"].toInt() + json["limit"].toInt()));

	return playlists;
}

Playlist Spotify::playlist(const QString &playlistId)
{
	return Playlist(getAsObject(QString("playlists/%1").arg(playlistId)));
}

QString Spotify::editPlaylist(const Playlist &playlist)
{
	QVariantMap body({
		{"name", playlist.name},
		{"public", playlist.isPublic},
		{"collaborative", playlist.collaborative},
		{"description", playlist.description}
	});
	return put(QString("playlists/%1").arg(playlist.id), &body);
}

QString Spotify::addToPlaylist(const QString &playlistId, const QString &trackId)
{
	return post(QString("playlists/%1/tracks?uris=%2")
		.arg(playlistId).arg(trackId));
}

QString Spotify::removeFromPlaylist(const QString &playlistId, const QString &trackId, int pos)
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

	return del(QString("playlists/%1/tracks").arg(playlistId), json);
}
