#include "track.hpp"

using namespace spt;

Track::Track(const QJsonObject &item)
{
	// If album is a string, track is loaded from cache
	if (item["album"].isString())
	{
		id			= item["id"].toString();
		album		= item["album"].toString();
		albumId		= item["albumId"].toString();
		artist		= item["artist"].toString();
		artistId	= item["artistId"].toString();
		name		= item["name"].toString();
		duration	= item["duration"].toInt();
		image		= item["image"].toString();
		isLocal		= item["isLocal"].toBool();
		addedAt		= QDateTime::fromString(item["addedAt"].toString());
		return;
	}
	// Object that contains the actual track object
	auto track = item.contains("track") ? item["track"].toObject() : item;
	// All properties
	id = track.contains("id")
		? track["id"].toString()
		: "0";
	album = track.contains("album")
		? track["album"].toObject()["name"].toString()
		: "(no album)";
	albumId	= track.contains("album")
		? track["album"].toObject()["id"].toString()
		: "0";
	artist = track.contains("artists")
		? track["artists"].toArray()[0].toObject()["name"].toString()
		: "(no artist)";
	artistId = track.contains("artists")
		? track["artists"].toArray()[0].toObject()["id"].toString()
		: "0";
	name = track.contains("name")
		? track["name"].toString()
		: "(no name)";
	duration = track.contains("duration_ms")
		? track["duration_ms"].toInt()
		: 0;
	image = track.contains("album")
		? track["album"].toObject()["images"].toArray()[2].toObject()["url"].toString()
		: "";
	addedAt = item.contains("added_at")
		? QDateTime::fromString(item["added_at"].toString(), Qt::DateFormat::ISODate)
		: QDateTime();
	isLocal = item["is_local"].toBool();
}

Track::Track()
{
	id = album = albumId = artist = artistId = name = image = QString();
	duration = 0;
	isLocal = false;
}

QJsonObject Track::toJson()
{
	return QJsonObject({
		QPair<QString, QString>("id", id),
		QPair<QString, QString>("album", album),
		QPair<QString, QString>("albumId", albumId),
		QPair<QString, QString>("artist", artist),
		QPair<QString, QString>("artistId", artistId),
		QPair<QString, QString>("name", name),
		QPair<QString, QString>("image", image),
		QPair<QString, int>("duration", duration),
		QPair<QString, bool>("isLocal", isLocal),
		QPair<QString, QString>("addedAt", addedAt.toString())
	});
}
