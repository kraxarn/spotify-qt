#include "playlist.hpp"

using namespace spt;

Playlist::Playlist(const QJsonObject &json)
{
	collaborative	= json["collaborative"].toBool();
	description		= json["description"].toString();
	id				= json["id"].toString();
	image = json.contains("image")
		? json["image"].toString()
		: json["images"].toArray()[0].toObject()["url"].toString();
	name = json["name"].toString();
	isPublic = Utils::getProperty(json, {
		"is_public", "public"
	}).toBool();
	tracks = json["tracks"].toObject();
	snapshot = Utils::getProperty(json, {
		"snapshot", "snapshot_id"
	}).toString();
	ownerId = json.contains("owner_id")
		? json["owner_id"].toString()
		: json.contains("ownerId")
			? json["owner_id"].toString()
			:  json["owner"].toObject()["id"].toString();
	ownerName = json.contains("owner_name")
		? json["owner_name"].toString()
		: json.contains("ownerName")
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
		QPair<QString, bool>("is_public", isPublic),
		QPair<QString, int>("total", jsonTracks.size()),
		QPair<QString, QJsonArray>("tracks", jsonTracks),
		QPair<QString, QString>("snapshot", snapshot),
		QPair<QString, QString>("owner_id", ownerId),
		QPair<QString, QString>("owner_name", ownerName)
	});
}