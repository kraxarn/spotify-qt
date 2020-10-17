#include "searchresults.hpp"

using namespace spt;

SearchResults::SearchResults(const QJsonObject &json)
{
	// Albums
	albums.reserve(json["albums"].toObject()["total"].toInt());
	for (auto album : json["albums"].toObject()["items"].toArray())
		albums.append(Album(album.toObject()));

	// Artists
	artists.reserve(json["artists"].toObject()["total"].toInt());
	for (auto artist : json["artists"].toObject()["items"].toArray())
		artists.append(Artist(artist.toObject()));

	// Playlists
	playlists.reserve(json["playlists"].toObject()["total"].toInt());
	for (auto playlist : json["playlists"].toObject()["items"].toArray())
		playlists.append(playlist.toObject());

	// Tracks
	tracks.reserve(json["tracks"].toObject()["total"].toInt());
	for (auto track : json["tracks"].toObject()["items"].toArray())
		tracks.append(Track(track.toObject()));
}

QJsonObject SearchResults::toJson() const
{
	QJsonArray jsonAlbums;
	for (auto &album : albums)
		jsonAlbums.append(album.toJson());

	QJsonArray jsonArtists;
	for (auto &artist : artists)
		jsonArtists.append(artist.toJson());

	QJsonArray jsonTracks;
	for (auto &track : tracks)
		jsonTracks.append(track.toJson());

	QJsonArray jsonPlaylists;
	for (auto &playlist : playlists)
		jsonPlaylists.append(playlist);

	return QJsonObject(
		{
			QPair<QString, QJsonArray>("albums", jsonAlbums),
			QPair<QString, QJsonArray>("artists", jsonArtists),
			QPair<QString, QJsonArray>("tracks", jsonTracks),
			QPair<QString, QJsonArray>("playlists", jsonPlaylists)
		}
	);
}
