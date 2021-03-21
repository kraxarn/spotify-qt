#include "playback.hpp"

spt::Playback::Playback(const QJsonObject &json)
{
	if (json["item"].isNull())
		return;

	progressMs = json["progress_ms"].toInt();
	JsonUtils::toJson(json["item"]).get_to(item);
	isPlaying = json["is_playing"].toBool();
	repeat = json["repeat_state"].toString();
	shuffle = json["shuffle_state"].toBool();
	contextUri = json["context"].isObject()
		? json["context"].toObject()["uri"].toString()
		: QString();
	contextType = json["context"].isObject()
		? json["context"].toObject()["type"].toString()
		: QString();

	JsonUtils::toJson(json["device"]).get_to(device);
}

QVariantMap spt::Playback::metadata() const
{
	QString itemName(isPlaying
		? QString::fromStdString(item.name)
		: QString());
	QStringList itemArtist(isPlaying
		? QString::fromStdString(item.artist)
		: QString());
	QString itemAlbum(isPlaying
		? QString::fromStdString(item.album)
		: QString());
	QString itemId(isPlaying
		? QString::fromStdString(item.id)
		: QString());
	auto itemDuration = isPlaying ? item.duration : 0;
	QString itemImage(isPlaying
		? QString::fromStdString(item.image)
		: QString());

	auto metadata = QVariantMap({
		{"xesam:title", itemName},
		{"xesam:artist", itemArtist},
		{"xesam:album", itemAlbum},
		{"xesam:albumArtist", itemArtist},
		{"xesam:url", QString("https://open.spotify.com/track/%1").arg(itemId)},
		{"mpris:length", (qulonglong) itemDuration * 1000},
		{"mpris:artUrl", itemImage},
		{"mpris:trackid", QString("spotify:track:%1").arg(itemId)},
	});
	return metadata;
}

QJsonObject spt::Playback::toJson() const
{
	return QJsonObject(
		{
			QPair<QString, int>("progress_ms", progressMs),
			QPair<QString, QJsonObject>("item", JsonUtils::toQtJson(item).object()),
			QPair<QString, bool>("is_playing", isPlaying),
			QPair<QString, QString>("repeat", repeat),
			QPair<QString, bool>("shuffle", shuffle)
		}
	);
}

int spt::Playback::volume() const
{
	return device.volume_percent;
}
