#pragma once

class TrayIcon;

#include "icon.hpp"
#include "mainwindow.hpp"
#include "spotify/spotify.hpp"
#include "spotify/playback.hpp"

#include <QSystemTrayIcon>
#include <QMenu>

class TrayIcon : private QSystemTrayIcon
{
	Q_OBJECT

public:
	TrayIcon(spt::Spotify *spotify, const Settings &settings, QObject *parent = nullptr);
	~TrayIcon();
	void message(const QString &message);
	void setPixmap(const QPixmap &pixmap);

private:
	spt::Playback	playback();
	QMenu 			*contextMenu;
	QAction			*playPause;
	QAction			*currentTrack;
	spt::Spotify	*spotify;
	const Settings	&settings;
};