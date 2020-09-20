#include "album.hpp"

using namespace spt;

Album::Album(const QJsonObject &json)
{
	id = json["id"].toString();
	albumGroup = json["album_group"].toString();
	image = json["images"].toArray()[2].toObject()["url"].toString();
	name = json["name"].toString();
	artist = json["artists"].toArray()[0].toObject()["name"].toString();
	releaseDate = QDateTime::fromString(json["release_date"].toString(), Qt::ISODate);
}