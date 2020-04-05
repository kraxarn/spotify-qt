#include "mediaplayer.hpp"

MediaPlayer::MediaPlayer(spt::Spotify *spotify, QObject *parent) : spotify(spotify), parent(parent),
	dBus(QDBusConnection::sessionBus()), QDBusAbstractAdaptor(parent)
{
}

MediaPlayer::~MediaPlayer() = default;

bool MediaPlayer::CanQuit() const
{
	return true;
}

bool MediaPlayer::CanRaise() const
{
	return true;
}

bool MediaPlayer::HasTrackList() const
{
	return false;
}

void MediaPlayer::Quit()
{
	QCoreApplication::quit();
}

void MediaPlayer::Raise()
{
	emit raisePlayer();
}

QString MediaPlayer::Identity() const
{
	return "spotify-qt";
}

QString MediaPlayer::DesktopEntry() const
{
	return "spotify-qt";
}

QStringList MediaPlayer::SupportedUriSchemas() const
{
	return QStringList() << "spotify";
}

QStringList MediaPlayer::SupportedMimeTypes() const
{
	return QStringList();
}