#include "service.hpp"
#include "mainwindow.hpp"

#ifdef USE_DBUS

#define SERVICE_NAME "org.mpris.MediaPlayer2.spotify-qt"
#define SERVICE_PATH "/org/mpris/MediaPlayer2"

mp::Service::Service(lib::spt::api &spotify, QObject *parent)
	: spotify(spotify),
	QObject(parent)
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
	{
		lib::log::warn("Failed to register D-Bus object");
	}
}

void mp::Service::signalPropertiesChange(const QObject *adaptor, const QVariantMap &properties)
{
	QDBusMessage msg = QDBusMessage::createSignal("/org/mpris/MediaPlayer2",
		"org.freedesktop.DBus.Properties", "PropertiesChanged");

	msg << adaptor->metaObject()->classInfo(0).value()
		<< properties
		<< QStringList();

	QDBusConnection::sessionBus().send(msg);
}

void mp::Service::metadataChanged()
{
	emit playerPlayer->emitMetadataChange();
}

void mp::Service::currentSourceChanged(const lib::spt::playback &playback)
{
	playerPlayer->setCurrentPlayback(playback);
	emit playerPlayer->currentSourceChanged();
	emit playerPlayer->totalTimeChanged();
}

void mp::Service::stateUpdated()
{
	emit playerPlayer->stateUpdated();
}

void mp::Service::seekableChanged()
{
	emit playerPlayer->seekableChanged(true);
}

void mp::Service::volumeChanged()
{
	emit playerPlayer->volumeChanged();
}

void mp::Service::tick(qint64 newPos)
{
	emit playerPlayer->tick(newPos);
}

auto mp::Service::currentPlayback() -> lib::spt::playback
{
	auto *mainWindow = dynamic_cast<MainWindow *>(parent());
	return mainWindow->currentPlayback();
}

auto mp::Service::isValid() -> bool
{
	return playerPlayer != nullptr;
}

#endif
