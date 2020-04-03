#pragma once

class MediaPlayer;

#include "../spotify/spotify.hpp"
#include "mediaplayerplayer.hpp"

#include <QDBusConnection>
#include <QDebug>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusAbstractAdaptor>
#include <QCoreApplication>

class MediaPlayer : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")

	Q_PROPERTY(bool CanQuit READ canQuit)
	Q_PROPERTY(bool CanRaise READ canQuit)
	Q_PROPERTY(QString Identity READ identity)
	Q_PROPERTY(QString DesktopEntry READ identity)
	Q_PROPERTY(QStringList SupportedUriSchemas READ supportedUriSchemas)
	Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes)
	Q_PROPERTY(bool HasTrackList READ hasTrackList)

public:
	explicit MediaPlayer(spt::Spotify *spotify, QObject *parent);

	bool canQuit() const;
	bool hasTrackList() const;
	QString identity() const;
	QStringList supportedUriSchemas() const;
	QStringList supportedMimeTypes() const;

public slots:
	Q_NOREPLY void Quit();
	Q_NOREPLY void Raise();

private:
	QDBusConnection		dBus;
	spt::Spotify		*spotify;
	QObject				*parent;
};