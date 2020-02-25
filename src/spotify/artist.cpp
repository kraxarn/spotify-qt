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
QVector<Track> Artist::topTracks(Spotify &spotify)
{
	auto json = spotify.get(QString("artists/%1/top-tracks?country=from_token").arg(id));
	auto items = json["tracks"].toArray();
	QVector<Track> tracks;
	tracks.reserve(items.size());
	for (auto item : items)
		tracks.append(Track(item.toObject()));
	return tracks;
}