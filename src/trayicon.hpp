#pragma once

class TrayIcon;

#include "mainwindow.hpp"
#include "lib/spotify/playback.hpp"
#include "spotify/spotify.hpp"
#include "util/icon.hpp"

#include <QMenu>
#include <QSystemTrayIcon>

class TrayIcon: private QSystemTrayIcon
{
Q_OBJECT

public:
	TrayIcon(spt::Spotify *spotify, const lib::settings &settings, QObject *parent = nullptr);
	~TrayIcon() override;

	void message(const QString &message);
	void setPixmap(const QPixmap &pixmap);

private:
	lib::spt::playback playback();

	QMenu *contextMenu;
	QAction *playPause;
	QAction *currentTrack;
	spt::Spotify *spotify;
	const lib::settings &settings;

	std::function<void(const std::string &result)> callback;
};