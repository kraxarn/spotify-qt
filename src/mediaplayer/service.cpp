#include "service.hpp"

#ifdef USE_DBUS

using namespace mp;

#define SERVICE_NAME "org.mpris.MediaPlayer2.spotify-qt"
#define SERVICE_PATH "/org/mpris/MediaPlayer2"

Service::Service(spt::Spotify *spotify, QObject *parent)
	: spotify(spotify), playerPlayer(nullptr), QObject(parent)
{
	if (!QDBusConnection::sessionBus().registerService(SERVICE_NAME))
	{
		lib::log::warn("Failed to register D-Bus service, is another instance running?");
		return;
	}
	new MediaPlayer(spotify, this);
	playerPlayer = new MediaPlayerPlayer(spotify, this);
	if (!QDBusConnection::sessionBus()
		.registerObject(SERVICE_PATH, this, QDBusConnection::ExportAdaptors))
		lib::log::warn("Failed to register D-Bus object");
}

Service::~Service() = default;

void Service::signalPropertiesChange(const QObject *adaptor, const QVariantMap &properties)
{
	QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
		"org.freedesktop.DBus.Properties", "PropertiesChanged");

	msg << adaptor->metaObject()->classInfo(0).value();
	msg << properties;
	msg << QStringList();

	QDBusConnection::sessionBus().send(msg);
}

void Service::metadataChanged()
{
	emit playerPlayer->emitMetadataChange();
}

void Service::currentSourceChanged(const spt::Playback &playback)
{
	playerPlayer->setCurrentPlayback(playback);
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
}

spt::Playback Service::currentPlayback()
{
	return ((MainWindow *) parent())->currentPlayback();
}

bool Service::isValid()
{
	return playerPlayer != nullptr;
}

#endif