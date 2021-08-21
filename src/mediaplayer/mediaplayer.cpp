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

auto MediaPlayer::canQuit() const -> bool
{
	return true;
}

auto MediaPlayer::identity() const -> QString
{
	return QCoreApplication::applicationName();
}

auto MediaPlayer::supportedUriSchemas() const -> QStringList
{
	return QStringList("spotify");
}

auto MediaPlayer::supportedMimeTypes() const -> QStringList
{
	return {};
}

auto MediaPlayer::hasTrackList() const -> bool
{
	return false;
}

#endif
