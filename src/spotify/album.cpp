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

QJsonObject Album::toJson() const
{
	return QJsonObject(
		{
			QPair<QString, QString>("id", id),
			QPair<QString, QString>("album_group", albumGroup),
			QPair<QString, QString>("image", image),
			QPair<QString, QString>("name", name),
			QPair<QString, QString>("artist", artist),
			QPair<QString, QString>("release_date", QLocale::system().toString(releaseDate)),
		}
	);
}