#include "artist.hpp"

using namespace spt;

Artist::Artist(const QJsonObject &json)
{
	id			= json["id"].toString();
	followers	= json["followers"].toObject()["total"].toInt();
	popularity	= json["popularity"].toInt();
	genres		= QStringList();
	name		= json["name"].toString();
	image		= json["images"].toArray()[1].toObject()["url"].toString();
	for (auto genre : json["genres"].toArray())
		genres.append(genre.toString());
}

QVector<Track> Artist::topTracks(Spotify &spotify) const
{
	auto json = spotify.get(QString("artists/%1/top-tracks?country=from_token").arg(id));
	auto items = json["tracks"].toArray();
	QVector<Track> tracks;
	tracks.reserve(items.size());
	for (auto item : items)
		tracks.append(Track(item.toObject()));
	return tracks;
}

QVector<Album> Artist::albums(Spotify &spotify) const
{
	auto json = spotify.get(QString("artists/%1/albums?country=from_token").arg(id));
	auto items = json["items"].toArray();
	QVector<Album> albums;
	albums.reserve(items.size());
	for (auto item : items)
		albums.append(Album(item.toObject()));
	return albums;
}

QVector<Artist> Artist::relatedArtists(Spotify &spotify) const
{
	auto json = spotify.get(QString("artists/%1/related-artists").arg(id));
	auto items = json["artists"].toArray();
	QVector<Artist> artists;
	artists.reserve(items.size());
	for (auto item : items)
		artists.append(Artist(item.toObject()));
	return artists;
}

QString Artist::itemName()
{
	return name;
}

QString Artist::itemId()
{
	return QString();
}
