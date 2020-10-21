#include "mediaplayer.hpp"

MediaPlayer::MediaPlayer(spt::Spotify *spotify, QObject *parent)
	: spotify(spotify), parent(parent)
#ifdef WITH_DBUS
	, dBus(QDBusConnection::sessionBus()), QDBusAbstractAdaptor(parent)
#endif
{
}

#ifdef WITH_DBUS
void MediaPlayer::Quit() const
{
	QCoreApplication::quit();
}

void MediaPlayer::Raise() const
{
	if (parent != nullptr)
		((QWidget *) parent)->raise();
}
#endif

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

bool MediaPlayer::hasTrackList() const
{
	return false;
}
