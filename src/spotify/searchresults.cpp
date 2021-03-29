#include "searchresults.hpp"

using namespace spt;

SearchResults::SearchResults(const QJsonObject &json)
{
	// Albums
	albums.reserve(json["albums"].toObject()["total"].toInt());
	for (auto album : json["albums"].toObject()["items"].toArray())
		albums.append(JsonUtils::toJson(album).get<lib::spt::album>());

	// Artists
	artists.reserve(json["artists"].toObject()["total"].toInt());
	for (auto artist : json["artists"].toObject()["items"].toArray())
		artists.append(JsonUtils::toJson(artist).get<lib::spt::artist>());

	// Playlists
	playlists.reserve(json["playlists"].toObject()["total"].toInt());
	for (auto playlist : json["playlists"].toObject()["items"].toArray())
		playlists.append(JsonUtils::toJson(playlist).get<lib::spt::playlist>());

	// Tracks
	tracks.reserve(json["tracks"].toObject()["total"].toInt());
	for (auto track : json["tracks"].toObject()["items"].toArray())
		tracks.append(JsonUtils::toJson(track).get<lib::spt::track>());
}

QJsonObject SearchResults::toJson() const
{
	QJsonArray jsonAlbums;
	for (auto &album : albums)
		jsonAlbums.append(JsonUtils::toQtJson(album).object());

	QJsonArray jsonArtists;
	for (auto &artist : artists)
		jsonArtists.append(JsonUtils::toQtJson(artist).object());

	QJsonArray jsonTracks;
	for (auto &track : tracks)
		jsonTracks.append(JsonUtils::toQtJson(track).object());

	QJsonArray jsonPlaylists;
	for (auto &playlist : playlists)
		jsonPlaylists.append(JsonUtils::toQtJson(playlist).object());

	return QJsonObject(
		{
			QPair<QString, QJsonArray>("albums", jsonAlbums),
			QPair<QString, QJsonArray>("artists", jsonArtists),
			QPair<QString, QJsonArray>("tracks", jsonTracks),
			QPair<QString, QJsonArray>("playlists", jsonPlaylists)
		}
	);
}
