#include "playlist.hpp"

using namespace spt;

Playlist::Playlist(const QJsonObject &json)
{
	// If image exists, it's loaded from cache
	auto fromCache = json.contains("image");

	collaborative	= json["collaborative"].toBool();
	description		= json["description"].toString();
	id				= json["id"].toString();
	image			= fromCache
		? json["image"].toString()
		: json["images"].toArray()[0].toObject()["url"].toString();
	name			= json["name"].toString();
	isPublic		= json["public"].toBool();
	tracks			= json["tracks"].toObject();
	snapshot		= json[fromCache ? "snapshot" : "snapshot_id"].toString();
	ownerId			= fromCache
		? json["ownerId"].toString()
		: json["owner"].toObject()["id"].toString();
	ownerName		= fromCache
		? json["ownerName"].toString()
		: json["owner"].toObject()["display_name"].toString();
}

QVector<Track> Playlist::loadTracks(Spotify &spotify) const
{
	// Allocate memory for all tracks
	QVector<Track> trackList;
	trackList.reserve(tracks["total"].toInt());
	// Load tracks
	auto href = tracks["href"].toString();
	loadTracksFromUrl(trackList, href, 0, spotify);
	// Return final track list
	return trackList;
}

bool Playlist::loadTracksFromUrl(QVector<Track> &trackList, QString &url, int offset, Spotify &spotify)
{
	// Load tracks from api
	auto newUrl = url.remove(0, QString("https://api.spotify.com/v1/").length());
	auto current = spotify.get(newUrl);
	// Load from url
	auto items = current["items"].toArray();
	for (auto item : items)
		trackList.append(Track(item.toObject()));
	// Check if there's a next page
	auto nextPage = current["next"].toString();
	if (!nextPage.isEmpty())
		loadTracksFromUrl(trackList, nextPage, offset + items.size(), spotify);
	return true;
}

QJsonObject Playlist::toJson(Spotify &spotify)
{
	// Load tracks to put in JSON
	QJsonArray jsonTracks;
	for (auto &track : loadTracks(spotify))
		jsonTracks.append(track.toJson());
	return QJsonObject({
		QPair<QString, bool>("collaborative", collaborative),
		QPair<QString, QString>("description", description),
		QPair<QString, QString>("id", id),
		QPair<QString, QString>("image", image),
		QPair<QString, QString>("name", name),
		QPair<QString, bool>("isPublic", isPublic),
		QPair<QString, int>("total", jsonTracks.size()),
		QPair<QString, QJsonArray>("tracks", jsonTracks),
		QPair<QString, QString>("snapshot", snapshot),
		QPair<QString, QString>("ownerId", ownerId),
		QPair<QString, QString>("ownerName", ownerName)
	});
}