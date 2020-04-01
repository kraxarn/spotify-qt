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
	if (spotify->currentPlayback().isPlaying)
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

bool MediaPlayerPlayer::canControl()
{
	return true;
}
QMap<QString, QVariant> MediaPlayerPlayer::metadata()
{
	return spotify->currentPlayback().metadata();
}

float MediaPlayerPlayer::getVolume()
{
	return (float) spotify->currentPlayback().volume / 100.f;
}

void MediaPlayerPlayer::setVolume(float value)
{
	spotify->setVolume((int) (value * 100));
}

int MediaPlayerPlayer::position()
{
	return spotify->currentPlayback().progressMs;
}
QString MediaPlayerPlayer::playbackStatus()
{
	return spotify->currentPlayback().isPlaying ? "Playing" : "Paused";
}
