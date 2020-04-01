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

	Q_PROPERTY(bool CanControl READ canControl)
	Q_PROPERTY(bool CanGoNext READ canControl)
	Q_PROPERTY(bool CanGoPrevious READ canControl)
	Q_PROPERTY(bool CanPause READ canControl)
	Q_PROPERTY(bool CanPlay READ canControl)
	Q_PROPERTY(bool CanSeek READ canControl)
	Q_PROPERTY(QMap<QString, QVariant> Metadata READ metadata)

public:
	explicit MediaPlayerPlayer(spt::Spotify *spotify, QObject *parent);

	bool canControl();
	QMap<QString, QVariant> metadata();

public slots:
	Q_NOREPLY void Next();
	Q_NOREPLY void Pause();
	Q_NOREPLY void Play();
	Q_NOREPLY void PlayPause();
	Q_NOREPLY void Previous();
	Q_NOREPLY void Seek(qint64 offset);
	Q_NOREPLY void SetPosition(QDBusObjectPath trackId, qint64 position);
	Q_NOREPLY void Stop();

private:
	QDBusConnection	dBus;
	spt::Spotify	*spotify;
};