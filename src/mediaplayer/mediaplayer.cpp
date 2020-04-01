#include "mediaplayer.hpp"

#define SERVICE_NAME "org.mpris.MediaPlayer2.spotify-qt"
#define SERVICE_INTERFACE "org.mpris.MediaPlayer2"
#define SERVICE_PATH "/org/mpris/MediaPlayer2"

MediaPlayer::MediaPlayer(spt::Spotify *spotify, QWidget *parent) : spotify(spotify), parent(parent),
	dBus(QDBusConnection::sessionBus())
{
	if (!dBus.registerObject(SERVICE_PATH, SERVICE_INTERFACE, this, QDBusConnection::ExportAllContents))
		qWarning() << "object error:" << dBus.lastError().message();

	if (!dBus.registerObject(SERVICE_PATH, "org.mpris.MediaPlayer2.Player", this, QDBusConnection::ExportAllContents))
		qWarning() << "object2 error:" << dBus.lastError().message();

	if (!dBus.registerService(SERVICE_NAME))
		qWarning() << "service error:" << dBus.lastError().message();

	//player = new MediaPlayerPlayer(spotify, parent);
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
