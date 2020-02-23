#include "track.hpp"

using namespace spt;

Track::Track(const QJsonObject &item)
{
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
		? track["album"].toObject()["images"].toArray()[0].toObject()["url"].toString()
		: "";
	isLocal = item["is_local"].toBool();
}

Track::Track()
{
	id = album = albumId = artist = artistId = name = image = QString();
	duration = 0;
	isLocal = false;
}
