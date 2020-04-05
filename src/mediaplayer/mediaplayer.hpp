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
#include <QStringList>

class MediaPlayer : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")

	Q_PROPERTY(bool CanQuit READ CanQuit CONSTANT)
	Q_PROPERTY(bool CanRaise READ CanRaise CONSTANT)
	Q_PROPERTY(bool HasTrackList READ HasTrackList CONSTANT)

	Q_PROPERTY(QString Identity READ Identity CONSTANT)
	Q_PROPERTY(QString DesktopEntry READ DesktopEntry CONSTANT)

	Q_PROPERTY(QStringList SupportedUriSchemas READ SupportedUriSchemas CONSTANT)
	Q_PROPERTY(QStringList SupportedMimeTypes READ SupportedMimeTypes CONSTANT)

public:
	explicit MediaPlayer(spt::Spotify *spotify, QObject *parent);
	~MediaPlayer() override;

	bool CanQuit() const;
	bool CanRaise() const;
	bool HasTrackList() const;

	QString Identity() const;
	QString DesktopEntry() const;

	QStringList SupportedUriSchemas() const;
	QStringList SupportedMimeTypes() const;

public slots:
	void Quit();
	void Raise();

signals:
	void raisePlayer();

private:
	QDBusConnection		dBus;
	spt::Spotify		*spotify;
	QObject				*parent;
};