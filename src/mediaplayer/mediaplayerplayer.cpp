#include "mediaplayerplayer.hpp"

#ifdef USE_DBUS

using namespace mp;

MediaPlayerPlayer::MediaPlayerPlayer(spt::Spotify *spotify, QObject *parent)
	: spotify(spotify),
	dBus(QDBusConnection::sessionBus()),
	QDBusAbstractAdaptor(parent)
{
	callback = [](const QString &result)
	{
		// We trust that the error message has already been logged somewhere
	};
}

void MediaPlayerPlayer::Next() const
{
	spotify->next(callback);
}

void MediaPlayerPlayer::Pause() const
{
	spotify->pause(callback);
}

void MediaPlayerPlayer::Play() const
{
	spotify->resume(callback);
}

void MediaPlayerPlayer::PlayPause() const
{
	if (currentPlayback().isPlaying)
		spotify->pause(callback);
	else
		spotify->resume(callback);
}

void MediaPlayerPlayer::Previous() const
{
	spotify->previous(callback);
}

void MediaPlayerPlayer::Seek(qint64 offset) const
{
	spotify->seek(offset, callback);
}

void MediaPlayerPlayer::SetPosition(const QDBusObjectPath &trackId, qint64 position) const
{
	spotify->seek(position, callback);
}

void MediaPlayerPlayer::Stop() const
{
	spotify->pause(callback);
}

bool MediaPlayerPlayer::canControl() const
{
	return true;
}

QMap<QString, QVariant> MediaPlayerPlayer::metadata() const
{
	return currentPlayback().metadata();
}

double MediaPlayerPlayer::getVolume() const
{
	return currentPlayback().volume() / 100.0;
}

void MediaPlayerPlayer::setVolume(double value) const
{
	spotify->setVolume((int) (value * 100), callback);
}

qint64 MediaPlayerPlayer::position() const
{
	return currentPlayback().progressMs * 1000;
}

QString MediaPlayerPlayer::playbackStatus() const
{
	return currentPlayback().isPlaying ? "Playing" : "Paused";
}

void MediaPlayerPlayer::OpenUri(QString uri) const
{
	// TODO
	lib::log::warn("Tried to open \"{}\", but not implemented yet", uri.toStdString());
}

double MediaPlayerPlayer::playbackRate() const
{
	return 1.0;
}

void MediaPlayerPlayer::setPlaybackRate(double value) const
{
	lib::log::warn("Changing playback rate is not supported");
}

bool MediaPlayerPlayer::shuffle() const
{
	return currentPlayback().shuffle;
}

void MediaPlayerPlayer::setShuffle(bool value) const
{
	spotify->setShuffle(value, callback);
}

void MediaPlayerPlayer::emitMetadataChange() const
{
	QVariantMap properties;
	properties["Metadata"] = currentPlayback().metadata();
	Service::signalPropertiesChange(this, properties);
}

void MediaPlayerPlayer::currentSourceChanged() const
{
	QVariantMap properties;
	properties["Metadata"] = currentPlayback().metadata();
	properties["PlaybackStatus"] = currentPlayback().isPlaying ? "Playing" : "Paused";
	//properties["CanSeek"] = true;
	Service::signalPropertiesChange(this, properties);
}

void MediaPlayerPlayer::stateUpdated() const
{
	QVariantMap properties;
	properties["PlaybackStatus"] = currentPlayback().isPlaying ? "Playing" : "Paused";
	//properties["CanPause"] = true;
	Service::signalPropertiesChange(this, properties);
}

void MediaPlayerPlayer::totalTimeChanged() const
{
	QVariantMap properties;
	properties["Metadata"] = currentPlayback().metadata();
	Service::signalPropertiesChange(this, properties);
}

void MediaPlayerPlayer::seekableChanged(bool seekable) const
{
	QVariantMap properties;
	properties["CanSeek"] = seekable;
	Service::signalPropertiesChange(this, properties);
}

void MediaPlayerPlayer::volumeChanged() const
{
	QVariantMap properties;
	properties["Volume"] = currentPlayback().volume() / 100.0;
	Service::signalPropertiesChange(this, properties);
}

void MediaPlayerPlayer::tick(qint64 newPos)
{
	emit Seeked(newPos * 1000);
}

void MediaPlayerPlayer::setCurrentPlayback(const spt::Playback &playback)
{
}

spt::Playback MediaPlayerPlayer::currentPlayback() const
{
	return ((Service *) parent())->currentPlayback();
}

#endif