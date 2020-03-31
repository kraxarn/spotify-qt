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