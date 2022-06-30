#include "mediaplayer/client.hpp"

#ifdef USE_DBUS

#include "mediaplayer/dbusservice.hpp"

#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>

MediaPlayer::Client::Client()
	: interface(SERVICE_NAME, SERVICE_PATH, QStringLiteral("org.mpris.MediaPlayer2.Player"))
{
}

auto MediaPlayer::Client::callMethod(const QString &name, const QVariant &arg) -> bool
{
	if (!interface.isValid())
	{
		qInfo().noquote() << invalidMessage();
		return false;
	}

	const auto response = arg.isValid()
		? interface.call(name, arg)
		: interface.call(name);

	return response.type() != QDBusMessage::ErrorMessage;
}

auto MediaPlayer::Client::invalidMessage() -> QString
{
	return QString("%1 is not running").arg(APP_NAME);
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

auto MediaPlayer::Client::openUri(const QString &trackUrl) -> bool
{
	return callMethod(QStringLiteral("OpenUri"), trackUrl);
}

#endif
