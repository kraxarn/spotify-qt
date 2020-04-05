#include "service.hpp"

using namespace mp;

#define SERVICE_NAME "org.mpris.MediaPlayer2.spotifyqt"
#define SERVICE_PATH "/org/mpris/MediaPlayer2"

Service::Service(spt::Spotify *spotify, QObject *parent) : spotify(spotify), QObject(parent)
{
	if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME))
	{
		qWarning() << "warning: failed to register d-bus service";
		return;
	}
	player 			= new MediaPlayer(spotify, this);
	playerPlayer	= new MediaPlayerPlayer(spotify, this);
	if (!QDBusConnection::sessionBus().registerObject(SERVICE_PATH, this, QDBusConnection::ExportAdaptors))
		qWarning() << "warning: failed to register d-bus object";
}

Service::~Service() = default;
/*{
	QDBusConnection::sessionBus().unregisterService(SERVICE_NAME);
}*/

void Service::metadataChanged()
{
	emit playerPlayer->playbackStatusChanged();
}

void Service::currentSourceChanged()
{
	emit playerPlayer->currentTrackChanged();
}

void Service::totalTimeChanged()
{
	emit playerPlayer->playbackStatusChanged();
}

void Service::stateChanged()
{
	emit playerPlayer->playbackStatusChanged();
}

void Service::seekableChanged()
{
	emit playerPlayer->canSeekChanged();
}

void Service::volumeChanged()
{
	emit playerPlayer->volumeChanged(spotify->currentPlayback().volume);
}

/*void Service::signalPropertiesChange(const QObject* adaptor, const QVariantMap& properties)
{
	QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
		"org.freedesktop.DBus.Properties", "PropertiesChanged" );

    msg << adaptor->metaObject()->classInfo(0).value();
    msg << properties;
    msg << QStringList();

    QDBusConnection::sessionBus().send(msg);
}

void Service::metadataChanged()
{
	emit playerPlayer->emitMetadataChange();
}

void Service::currentSourceChanged()
{
	emit playerPlayer->currentSourceChanged();
	emit playerPlayer->totalTimeChanged();
}

void Service::stateUpdated()
{
	emit playerPlayer->stateUpdated();
}

void Service::seekableChanged()
{
	emit playerPlayer->seekableChanged(true);
}

void Service::volumeChanged()
{
	emit playerPlayer->volumeChanged();
}

void Service::tick(qint64 newPos)
{
	emit playerPlayer->tick(newPos);
}*/

/*void Service::updateMetadata(const QVariantMap &metadata)
{
	//properties->updateMetadata(metadata);

	QVariantMap properties;
	properties["Metadata"] = metadata;

	auto msg = QDBusMessage::createSignal(SERVICE_PATH,
		"org.freedesktop.DBus.Properties", "PropertiesChanged");
	msg << "org.mpris.MediaPlayer2.Player";
	msg << metadata;
	msg << QStringList();
	QDBusConnection::sessionBus().send(msg);
}

void Service::updateSeeked(qint64 position)
{
	emit playerPlayer->Seeked(position);
}*/