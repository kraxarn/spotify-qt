#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "util/icon.hpp"

#include <QMenu>
#include <QSystemTrayIcon>

#ifdef USE_DBUS
#include "notifications/dbusnotifications.hpp"
#endif

class TrayIcon: public QSystemTrayIcon
{
Q_OBJECT

public:
	TrayIcon(lib::spt::api &spotify, const lib::settings &settings,
		const lib::cache &cache, QWidget *parent);
	~TrayIcon() override;

	void message(const QString &message);

	/**
	 * Notify about track, for example on track change
	 * @param track Track to show
	 * @param pixmap Album art
	 */
	void message(const lib::spt::track &track, const QPixmap &pixmap);

	void setPixmap(const QPixmap &pixmap);
	void setDefaultPixmap();

private:
	/**
	 * How long in milliseconds to show notifications about tracks
	 */
	static constexpr int messageTrackTimeout = 6000;

	auto playback() -> lib::spt::playback;

	QAction *previous = nullptr;
	QAction *playPause = nullptr;
	QAction *next = nullptr;

	QMenu *contextMenu = nullptr;

	QAction *currentTrack = nullptr;
	lib::spt::api &spotify;
	const lib::settings &settings;
	const lib::cache &cache;

#ifdef USE_DBUS
	DbusNotifications notifications;
#endif

	std::function<void(const std::string &result)> callback;

	void onPrevious(bool checked);
	void onPlayPause(bool checked);
	void onNext(bool checked);
	void onActivated(ActivationReason reason);
	void onMenuAboutToShow();
};
