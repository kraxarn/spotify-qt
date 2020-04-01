#include "mediaplayer.hpp"

MediaPlayer::MediaPlayer(spt::Spotify *spotify, QWidget *parent) : spotify(spotify), parent(parent),
	dBus(QDBusConnection::sessionBus())
{
	if (!dBus.registerObject(SERVICE_PATH, SERVICE_INTERFACE, this, QDBusConnection::ExportAllContents))
		qWarning() << "object error:" << dBus.lastError().message();

	if (!dBus.registerService(SERVICE_NAME))
		qWarning() << "service error:" << dBus.lastError().message();
}

MediaPlayer::~MediaPlayer()
{
	QDBusConnection::sessionBus().unregisterService(SERVICE_NAME);
}

void MediaPlayer::Quit()
{
	QCoreApplication::quit();
}

void MediaPlayer::Raise()
{
	if (parent != nullptr)
		parent->raise();
}

bool MediaPlayer::canQuit() const
{
	return true;
}
QString MediaPlayer::identity() const
{
	return "spotify-qt";
}

QStringList MediaPlayer::supportedUriSchemas() const
{
	return QStringList("spotify");
}

QStringList MediaPlayer::supportedMimeTypes() const
{
	return QStringList();
}
