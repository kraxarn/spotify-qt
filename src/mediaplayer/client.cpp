#include "mediaplayer/client.hpp"
#include "mediaplayer/dbusservice.hpp"

#include <QJsonDocument>
#include <QJsonObject>

MediaPlayer::Client::Client()
	: interface(SERVICE_NAME, SERVICE_PATH, QStringLiteral("org.mpris.MediaPlayer2.Player"))
{
}

auto MediaPlayer::Client::callMethod(const QString &name) -> bool
{
	if (!interface.isValid())
	{
		qInfo() << invalidMessage();
		return false;
	}

	return interface.call(name).type() != QDBusMessage::ErrorMessage;
}

auto MediaPlayer::Client::invalidMessage() -> QString
{
	return QString("%1 needs to be running").arg(APP_NAME);
}

auto MediaPlayer::Client::playPause() -> bool
{
	return callMethod(QStringLiteral("PlayPause"));
}

auto MediaPlayer::Client::previousTrack() -> bool
{
	return callMethod(QStringLiteral("Previous"));
}

auto MediaPlayer::Client::nextTrack() -> bool
{
	return callMethod(QStringLiteral("Next"));
}

auto MediaPlayer::Client::metadata() -> QString
{
	const auto prop = interface.property("Metadata");
	if (!prop.isValid())
	{
		return invalidMessage();
	}

	const auto json = QJsonObject::fromVariantMap(prop.value<QVariantMap>());
	return QJsonDocument(json).toJson(QJsonDocument::Compact);
}
