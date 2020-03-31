#include "playback.hpp"

spt::Playback::Playback()
{
}

spt::Playback::Playback(const QJsonObject &json)
{
	if (!json["item"].isNull())
	{
		progressMs	= json["progress_ms"].toInt();
		item 		= Track(json["item"].toObject());
		isPlaying 	= json["is_playing"].toBool();
		volume 		= json["device"].toObject()["volume_percent"].toInt();
		repeat		= json["repeat_state"].toString();
		shuffle		= json["shuffle_state"].toBool();
	}
}

QMap<QString, QVariant> spt::Playback::metadata()
{
	return QMap<QString, QVariant>({
		{ "xesam:title",		item.name },
		{ "xesam:artist",	QStringList(item.artist) },
		{ "xesam:album", 	item.album },
		{ "xesam:url", 		QString("https://open.spotify.com/track/%1").arg(item.id) },
		{ "mpris:length",	item.duration },
		{ "mpris:artUrl", 	item.image },
		{ "mpris:trackid", QString("spotify:track:%1").arg(item.id) }
	});
}
