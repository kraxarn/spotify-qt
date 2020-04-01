#include "mediaplayerplayer.hpp"

MediaPlayerPlayer::MediaPlayerPlayer(spt::Spotify *spotify, QObject *parent)
: spotify(spotify), dBus(QDBusConnection::sessionBus()), QDBusAbstractAdaptor(parent)
{
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
	if (parent == nullptr)
		return;
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