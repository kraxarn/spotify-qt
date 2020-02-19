#include "track.hpp"

using namespace spt;

Track::Track()
{
	addedAt = QDateTime();
	isLocal = false;
	track   = QJsonObject();
}

Track::Track(QJsonObject item)
{
	addedAt = QDateTime();
	isLocal = item["is_local"].toBool();
	track   = item.contains("track") ? item["track"].toObject() : item;
}

QString Track::id()
{
	return track.contains("id") ? track["id"].toString() : "0";
}

QString Track::album()
{
	return track.contains("album") ? track["album"].toString() : "(no album)";
}

QString Track::artist()
{
	return track.contains("artists")
		? track["artists"].toArray()[0].toObject()["name"].toString()
		: "(no artist)";
}

QString Track::name()
{
	return track.contains("name") ? track["name"].toString() : "(no name)";
}

int Track::duration()
{
	return track.contains("duration_ms") ? track["duration_ms"].toInt() : 0;
}

QString Track::image()
{
	return track["album"].toObject()["images"].toArray()[0].toObject()["url"].toString();
}
