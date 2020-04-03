#pragma once

class MediaPlayerPlayer;

#include "../spotify/spotify.hpp"
#include "../mainwindow.hpp"

#include <QDBusConnection>
#include <QDebug>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusAbstractAdaptor>
#include <QCoreApplication>

class MediaPlayerPlayer : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.Player")

	Q_PROPERTY(QString PlaybackStatus READ playbackStatus)
	Q_PROPERTY(double Rate READ playbackRate WRITE setPlaybackRate)
	Q_PROPERTY(QMap<QString, QVariant> Metadata READ metadata)
	Q_PROPERTY(double Volume READ getVolume WRITE setVolume)
	Q_PROPERTY(qint64 Position READ position)
	Q_PROPERTY(double MinimumRate READ playbackRate)
	Q_PROPERTY(double MaximumRate READ playbackRate)
	Q_PROPERTY(bool CanGoNext READ canControl)
	Q_PROPERTY(bool CanGoPrevious READ canControl)
	Q_PROPERTY(bool CanPlay READ canControl)
	Q_PROPERTY(bool CanPause READ canControl)
	Q_PROPERTY(bool CanSeek READ canControl)
	Q_PROPERTY(bool CanControl READ canControl)
	Q_PROPERTY(bool Shuffle READ shuffle WRITE setShuffle)

public:
	explicit MediaPlayerPlayer(spt::Spotify *spotify, QObject *parent);

	bool canControl();
	QMap<QString, QVariant> metadata();

	double getVolume();
	void setVolume(double value);

	qint64 position();

	QString playbackStatus();

	double playbackRate();
	void setPlaybackRate(double value);

	bool shuffle();
	void setShuffle(bool value);

signals:
	void Seeked(qint64 position);

public slots:
	Q_NOREPLY void Next();
	Q_NOREPLY void Previous();
	Q_NOREPLY void Pause();
	Q_NOREPLY void PlayPause();
	Q_NOREPLY void Stop();
	Q_NOREPLY void Play();
	Q_NOREPLY void Seek(qint64 offset);
	Q_NOREPLY void SetPosition(QDBusObjectPath trackId, qint64 position);
	Q_NOREPLY void OpenUri(QString uri);

private:
	QDBusConnection	dBus;
	spt::Spotify	*spotify;
};