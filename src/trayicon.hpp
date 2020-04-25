#pragma once

class TrayIcon;

#include "icon.hpp"
#include "mainwindow.hpp"
#include "spotify/spotify.hpp"

#include <QSystemTrayIcon>
#include <QMenu>

class TrayIcon : private QSystemTrayIcon
{
	Q_OBJECT

public:
	TrayIcon(spt::Spotify *spotify, QObject *parent = nullptr);
	~TrayIcon();
	void message(const QString &message);

private:
	bool			isPlaying();
	QMenu 			*contextMenu;
	QAction			*playPause;
	spt::Spotify	*spotify;
};