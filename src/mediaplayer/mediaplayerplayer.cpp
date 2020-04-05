#include "mediaplayerplayer.hpp"

using namespace mp;

MediaPlayerPlayer::MediaPlayerPlayer(spt::Spotify *spotify, QObject *parent)
: spotify(spotify), dBus(QDBusConnection::sessionBus()), QDBusAbstractAdaptor(parent)
{
	signalPropertiesChange("Volume", Volume());
}

MediaPlayerPlayer::~MediaPlayerPlayer() = default;

QString MediaPlayerPlayer::PlaybackStatus() const
{
	return spotify->currentPlayback().isPlaying ? "Playing" : "Paused";
}

bool MediaPlayerPlayer::CanGoNext() const
{
	return true;
}

void MediaPlayerPlayer::Next()
{
	emit next();
	spotify->next();
}

bool MediaPlayerPlayer::CanGoPrevious() const
{
	return true;
}

void MediaPlayerPlayer::Previous()
{
	emit previous();
	spotify->previous();
}

bool MediaPlayerPlayer::CanPause() const
{
	return true;
}

void MediaPlayerPlayer::Pause()
{
	spotify->pause();
}

void MediaPlayerPlayer::PlayPause()
{
	emit playPause();
	if (spotify->currentPlayback().isPlaying)
		spotify->pause();
	else
		spotify->resume();
}

void MediaPlayerPlayer::Stop()
{
	emit stop();
	spotify->pause();
}

bool MediaPlayerPlayer::CanPlay() const
{
	return true;
}

void MediaPlayerPlayer::Play()
{
	spotify->resume();
}

double MediaPlayerPlayer::Volume() const
{
	return spotify->currentPlayback().volume / 100.0;
}

void MediaPlayerPlayer::setVolume(double value)
{
	emit volumeChanged(value);
	spotify->setVolume((int) (value * 100));
	signalPropertiesChange("Volume", Volume());
}

QVariantMap MediaPlayerPlayer::Metadata() const
{
	return spotify->currentPlayback().metadata();
}

qlonglong MediaPlayerPlayer::Position() const
{
	return spotify->currentPlayback().progressMs;
}

void MediaPlayerPlayer::setPropertyPosition(int newPositionInMs)
{
	spotify->seek(newPositionInMs);
	emit Seeked(newPositionInMs * 1000);
}

double MediaPlayerPlayer::Rate() const
{
	return 1.0;
}

void MediaPlayerPlayer::setRate(double value)
{
	qWarning() << "warning: changing playback rate is not supported by spotify";
	emit rateChanged(value);
	signalPropertiesChange("Rate", Rate());
}

double MediaPlayerPlayer::MinimumRate() const
{
	return 1.0;
}

double MediaPlayerPlayer::MaximumRate() const
{
	return 1.0;
}

bool MediaPlayerPlayer::CanSeek() const
{
	return true;
}

bool MediaPlayerPlayer::CanControl() const
{
	return true;
}

void MediaPlayerPlayer::Seek(qlonglong offset)
{
	spotify->seek(offset);
}

void MediaPlayerPlayer::emitSeeked(int pos)
{
	emit Seeked(pos * 1000);
}

void MediaPlayerPlayer::SetPosition(const QDBusObjectPath &trackId, qlonglong pos)
{
	spotify->seek((int) pos / 1000);
}

void MediaPlayerPlayer::OpenUri(const QString &uri)
{
	// TODO
	qWarning() << "warning: tried to open" << uri << "but not implemented yet";
	Q_UNUSED(uri);
}

void MediaPlayerPlayer::playerSourceChanged()
{
	setCurrentTrack(0);
}

void MediaPlayerPlayer::playControlEnabledChanged()
{
	signalPropertiesChange("CanPause", CanPause());
	signalPropertiesChange("CanPlay", CanPlay());

	emit canPauseChanged();
	emit canPlayChanged();
}

void MediaPlayerPlayer::skipBackwardControlEnabledChanged()
{
	signalPropertiesChange("CanGoPrevious", CanGoPrevious());
	emit canGoPreviousChanged();
}

void MediaPlayerPlayer::skipForwardControlEnabledChanged()
{
	signalPropertiesChange("CanGoNext", CanGoNext());
	emit canGoNextChanged();
}

void MediaPlayerPlayer::playerPlaybackStateChanged()
{
	signalPropertiesChange("PlaybackStatus", PlaybackStatus());
	emit playbackStatusChanged();
	playerIsSeekableChanged();
}

void MediaPlayerPlayer::playerIsSeekableChanged()
{
	signalPropertiesChange("CanSeek", CanSeek());
	emit canSeekChanged();
}

void MediaPlayerPlayer::audioPositionChanged()
{
	setPropertyPosition(spotify->currentPlayback().progressMs);
}

void MediaPlayerPlayer::audioDurationChanged()
{
	signalPropertiesChange("Metadata", Metadata());
	skipBackwardControlEnabledChanged();
	skipForwardControlEnabledChanged();
	playerPlaybackStateChanged();
	playerIsSeekableChanged();
	setPropertyPosition(spotify->currentPlayback().progressMs);
}

void MediaPlayerPlayer::playerVolumeChanged()
{
	setVolume(spotify->currentPlayback().volume / 100.0);
}

int MediaPlayerPlayer::currentTrack() const
{
	return 0;
}

void MediaPlayerPlayer::setCurrentTrack(int newTrackPosition)
{
	emit currentTrackChanged();
}

QVariantMap MediaPlayerPlayer::getMetadataOfCurrentTrack()
{
	return spotify->currentPlayback().metadata();
}

int MediaPlayerPlayer::mediaPlayerPresent() const
{
	return 1;
}

bool MediaPlayerPlayer::showProgressOnTaskBar() const
{
	return false;
}

void MediaPlayerPlayer::setShowProgressOnTaskBar(bool value)
{
}

void MediaPlayerPlayer::setMediaPlayerPresent(int status)
{
	emit mediaPlayerPresentChanged();
	signalPropertiesChange("CanGoNext", CanGoNext());
	signalPropertiesChange("CanGoPrevious", CanGoPrevious());
	signalPropertiesChange("CanPause", CanPause());
	signalPropertiesChange("CanPlay", CanPlay());
	emit canGoNextChanged();
	emit canGoPreviousChanged();
	emit canPauseChanged();
	emit canPlayChanged();
}

void MediaPlayerPlayer::signalPropertiesChange(const QString &property, const QVariant &value)
{
	QVariantMap properties;
	properties[property] = value;
	const int ifaceIndex = metaObject()->indexOfClassInfo("D-Bus Interface");
	QDBusMessage msg = QDBusMessage::createSignal(
		QStringLiteral("/org/mpris/MediaPlayer2"),
		QStringLiteral("org.freedesktop.DBus.Properties"),
		QStringLiteral("PropertiesChanged"));
	msg << QLatin1String(metaObject()->classInfo(ifaceIndex).value());
	msg << properties;
	msg << QStringList();

	QDBusConnection::sessionBus().send(msg);
}


/*bool MediaPlayerPlayer::shuffle() const
{
	return spotify->currentPlayback().shuffle;
}

void MediaPlayerPlayer::setShuffle(bool value) const
{
	spotify->setShuffle(value);
}

void MediaPlayerPlayer::emitMetadataChange() const
{
	QVariantMap properties;
	properties["Metadata"] = spotify->currentPlayback().metadata();
	Service::signalPropertiesChange(this, properties);
}

void MediaPlayerPlayer::currentSourceChanged() const
{
	QVariantMap properties;
	properties["Metadata"] = spotify->currentPlayback().metadata();
	properties["CanSeek"] = true;
	Service::signalPropertiesChange(this, properties);
}

void MediaPlayerPlayer::stateUpdated() const
{
	QVariantMap properties;
	properties["PlaybackStatus"] = spotify->currentPlayback().isPlaying ? "Playing" : "Paused";
	properties["CanPause"] = true;
	Service::signalPropertiesChange(this, properties);
}

void MediaPlayerPlayer::totalTimeChanged() const
{
	QVariantMap properties;
	properties["Metadata"] = spotify->currentPlayback().metadata();
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
	properties["Volume"] = spotify->currentPlayback().volume / 100.0;
	Service::signalPropertiesChange(this, properties);
}

void MediaPlayerPlayer::tick(qint64 newPos)
{
	emit Seeked(newPos * 1000);
}*/