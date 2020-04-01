#include "service.hpp"

using namespace mp;

#define SERVICE_NAME "org.mpris.MediaPlayer2.spotify-qt"
#define SERVICE_INTERFACE "org.mpris.MediaPlayer2"
#define SERVICE_PATH "/org/mpris/MediaPlayer2"

Service::Service(spt::Spotify *spotify, QObject *parent) : QObject(parent)
{
	QDBusConnection::sessionBus().registerService(SERVICE_NAME);
	QDBusConnection::sessionBus().registerObject(SERVICE_PATH, SERVICE_INTERFACE, this);

	player 			= new MediaPlayer(spotify, this);
	playerPlayer	= new MediaPlayerPlayer(spotify, this);
}

Service::~Service()
{
	QDBusConnection::sessionBus().unregisterService(SERVICE_NAME);
}