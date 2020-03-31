#pragma once

#include "spotify/spotify.hpp"

#include <QDBusConnection>
#include <QDebug>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusAbstractAdaptor>

#define SERVICE_NAME "org.mpris.MediaPlayer2.spotify-qt"
#define SERVICE_INTERFACE "org.mpris.MediaPlayer2"
#define SERVICE_PLAYER_INTERFACE "org.mpris.MediaPlayer2.Player"
#define SERVICE_PATH "/org/mpris/MediaPlayer2"

class MediaPlayer : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", SERVICE_NAME)

public:
	explicit MediaPlayer(spt::Spotify *spotify);
	virtual ~MediaPlayer();

public slots:
	// Methods?
	void testSlot();

private:
	QDBusConnection	dBus;
	spt::Spotify	*spotify;
};