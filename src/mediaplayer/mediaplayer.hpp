#pragma once

#include "../spotify/spotify.hpp"
#include "mediaplayerplayer.hpp"

#include <QDBusConnection>
#include <QDebug>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusAbstractAdaptor>
#include <QCoreApplication>

class MediaPlayer : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")

	Q_PROPERTY(bool CanQuit READ canQuit)
	Q_PROPERTY(bool CanRaise READ canQuit)
	Q_PROPERTY(QString Identity READ identity)
	Q_PROPERTY(QStringList SupportedUriSchemas READ supportedUriSchemas)
	Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes)

public:
	explicit MediaPlayer(spt::Spotify *spotify, QWidget *parent = nullptr);
	~MediaPlayer() override;

	bool canQuit() const;
	QString identity() const;
	QStringList supportedUriSchemas() const;
	QStringList supportedMimeTypes() const;

public slots:
	Q_NOREPLY void Quit();
	Q_NOREPLY void Raise();

private:
	QDBusConnection		dBus;
	spt::Spotify		*spotify;
	QWidget				*parent;
	MediaPlayerPlayer	*player;
};