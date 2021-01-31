#include "track.hpp"

using namespace spt;

Track::Track(const QJsonObject &item)
{
	// If album is a string, track is loaded from cache
	if (item["album"].isString())
	{
		id = item["id"].toString();
		album = item["album"].toString();
		albumId = JsonUtils::getProperty(item, {
			"album_id", "albumId"
		}).toString();
		artist = item["artist"].toString();
		artistId = JsonUtils::getProperty(item, {
			"artist_id", "artistId"
		}).toString();
		name = item["name"].toString();
		duration = item["duration"].toInt();
		image = item["image"].toString();
		isLocal = JsonUtils::getProperty(item, {
			"is_local", "isLocal"
		}).toBool();
		addedAt = QDateTime::fromString(JsonUtils::getProperty(item, {
			"added_at", "addedAt"
		}).toString(), Qt::ISODate);
		isPlayable = item["is_playable"].toBool(true);
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
	albumId = track.contains("album")
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
		: item.contains("played_at")
			? QDateTime::fromString(item["played_at"].toString(), Qt::DateFormat::ISODate)
			: QDateTime();
	isLocal = item["is_local"].toBool();
	isPlayable = track["is_playable"].toBool(true);

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
	if (addedAt == QDateTime(QDate(1970, 1, 1).startOfDay(Qt::UTC)))
		addedAt = QDateTime();
#endif
}

Track::Track()
{
	id = album = albumId = artist = artistId = name = image = QString();
	duration = 0;
	isLocal = false;
	isPlayable = true;
}

QJsonObject Track::toJson() const
{
	return QJsonObject({
		QPair<QString, QString>("id", id),
		QPair<QString, QString>("album", album),
		QPair<QString, QString>("album_id", albumId),
		QPair<QString, QString>("artist", artist),
		QPair<QString, QString>("artist_id", artistId),
		QPair<QString, QString>("name", name),
		QPair<QString, QString>("image", image),
		QPair<QString, int>("duration", duration),
		QPair<QString, bool>("is_local", isLocal),
		QPair<QString, bool>("is_playable", isPlayable),
		QPair<QString, QString>("added_at", addedAt.toString(Qt::DateFormat::ISODate))
	});
}