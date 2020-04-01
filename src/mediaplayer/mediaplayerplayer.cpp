#include "mediaplayerplayer.hpp"

#define SERVICE_NAME "org.mpris.MediaPlayer2.spotify-qt"
#define SERVICE_INTERFACE "org.mpris.MediaPlayer2.Player"
#define SERVICE_PATH "/org/mpris/MediaPlayer2"

MediaPlayerPlayer::MediaPlayerPlayer(spt::Spotify *spotify, QWidget *parent) : spotify(spotify), parent(parent),
	dBus(QDBusConnection::sessionBus())
{
	if (!dBus.registerObject(SERVICE_PATH, SERVICE_INTERFACE, this, QDBusConnection::ExportAllContents))
		qWarning() << "object error:" << dBus.lastError().message();
}

MediaPlayerPlayer::~MediaPlayerPlayer()
{
	QDBusConnection::sessionBus().unregisterService(SERVICE_NAME);
}

void MediaPlayerPlayer::Next()
{
	spotify->next();
}

void MediaPlayerPlayer::Pause()
{
	spotify->pause();
}

void MediaPlayerPlayer::Play()
{
	spotify->resume();
}

void MediaPlayerPlayer::PlayPause()
{
	if (((MainWindow*) parent)->current.isPlaying)
		spotify->pause();
	else
		spotify->resume();
}

void MediaPlayerPlayer::Previous()
{
	spotify->previous();
}

void MediaPlayerPlayer::Seek(qint64 offset)
{
	spotify->seek(offset);
}

void MediaPlayerPlayer::SetPosition(QDBusObjectPath trackId, qint64 position)
{
	spotify->seek(position);
}

void MediaPlayerPlayer::Stop()
{
	spotify->pause();
}