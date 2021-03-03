#include "mediaplayerplayer.hpp"

#ifdef USE_DBUS

using namespace mp;

MediaPlayerPlayer::MediaPlayerPlayer(spt::Spotify *spotify, QObject *parent)
	: spotify(spotify), dBus(QDBusConnection::sessionBus()), QDBusAbstractAdaptor(parent)
{
}

std::function<void(const QString &)> MediaPlayerPlayer::getCallback(const QString &name) const
{
	return [name](const QString &status)
	{
		if (status.isEmpty())
			return;

		lib::log::error("MediaPlayerPlayer.{} failed: {}",
			name.toStdString(), status.toStdString());
	};
}

void MediaPlayerPlayer::Next() const
{
	spotify->next(getCallback("Next"));
}

void MediaPlayerPlayer::Pause() const
{
	spotify->pause(getCallback("Pause"));
}

void MediaPlayerPlayer::Play() const
{
	spotify->resume(getCallback("Play"));
}

void MediaPlayerPlayer::PlayPause() const
{
	if (currentPlayback().isPlaying)
		spotify->pause(getCallback("PlayPause"));
	else
		spotify->resume(getCallback("PlayPause"));
}

void MediaPlayerPlayer::Previous() const
{
	spotify->previous(getCallback("Previous"));
}

void MediaPlayerPlayer::Seek(qint64 offset) const
{
	spotify->seek(offset, getCallback("Seek"));
}

void MediaPlayerPlayer::SetPosition(const QDBusObjectPath &trackId, qint64 position) const
{
	spotify->seek(position, getCallback("SetPosition"));
}

void MediaPlayerPlayer::Stop() const
{
	spotify->pause(getCallback("Stop"));
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
	spotify->setVolume((int) (value * 100),
		getCallback("setVolume"));
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
	spotify->setShuffle(value, getCallback("setShuffle"));
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