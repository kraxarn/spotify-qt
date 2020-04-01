#include "mediaplayerplayer.hpp"

MediaPlayerPlayer::MediaPlayerPlayer(spt::Spotify *spotify, QObject *parent)
: spotify(spotify), dBus(QDBusConnection::sessionBus()), parent((QWidget*) parent), QDBusAbstractAdaptor(parent)
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
	if (((MainWindow*) parent)->isPlaying)
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
