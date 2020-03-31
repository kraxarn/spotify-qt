#include "mediaplayer.hpp"

MediaPlayer::MediaPlayer(spt::Spotify *spotify) : spotify(spotify),
	dBus(QDBusConnection::sessionBus())
{
	if (!dBus.registerObject(SERVICE_PATH, SERVICE_INTERFACE, this))
		qWarning() << "object error:" << dBus.lastError().message();

	if (!dBus.registerService(SERVICE_NAME))
		qWarning() << "service error:" << dBus.lastError().message();
}

MediaPlayer::~MediaPlayer()
{
	QDBusConnection::sessionBus().unregisterService(SERVICE_NAME);
}

void MediaPlayer::testSlot()
{
	qDebug() << "test slot";
}