#include "mediaplayer.hpp"

#ifdef USE_DBUS

MediaPlayer::MediaPlayer(lib::spt::api &spotify, QObject *parent)
	: spotify(spotify),
	dBus(QDBusConnection::sessionBus()),
	QDBusAbstractAdaptor(parent)
{
}

void MediaPlayer::Quit() const
{
	QCoreApplication::quit();
}

void MediaPlayer::Raise() const
{
	auto *window = Parent::findObject<QMainWindow>(parent());
	if (window != nullptr)
	{
		window->raise();
	}
}

bool MediaPlayer::canQuit() const
{
	return true;
}

QString MediaPlayer::identity() const
{
	return QCoreApplication::applicationName();
}

QStringList MediaPlayer::supportedUriSchemas() const
{
	return QStringList("spotify");
}

QStringList MediaPlayer::supportedMimeTypes() const
{
	return {};
}

bool MediaPlayer::hasTrackList() const
{
	return false;
}

#endif