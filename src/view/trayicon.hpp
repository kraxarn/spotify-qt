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
	TrayIcon(spt::Spotify *spotify, const lib::settings &settings, QWidget *parent);
	~TrayIcon() override;

	void message(const QString &message);
	void message(const QString &message, const QIcon &icon);

	void setPixmap(const QPixmap &pixmap);
	void setDefaultPixmap();

private:
	/**
	 * How long in milliseconds to show notifications with icons,
	 * for example when notifying about track changes
	 */
	static constexpr int messageIconTimeout = 6000;

	auto playback() -> lib::spt::playback;

	QMenu *contextMenu;
	QAction *playPause;
	QAction *currentTrack;
	spt::Spotify *spotify;
	const lib::settings &settings;

	std::function<void(const std::string &result)> callback;
};
