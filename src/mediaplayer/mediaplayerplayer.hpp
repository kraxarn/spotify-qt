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

	//Q_PROPERTY(bool CanQuit READ canQuit)

public:
	explicit MediaPlayerPlayer(spt::Spotify *spotify, QObject *parent);

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
	QWidget			*parent = nullptr;
};