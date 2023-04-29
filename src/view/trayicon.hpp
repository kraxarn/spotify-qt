#pragma once

#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"
#include "util/icon.hpp"

#include <QMenu>
#include <QSystemTrayIcon>

class TrayIcon: public QSystemTrayIcon
{
Q_OBJECT

public:
	TrayIcon(lib::spt::api &spotify, const lib::settings &settings, lib::cache &cache,
		const lib::http_client &httpClient, QWidget *parent);

	~TrayIcon() override;

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
	void showWindow();
	void message(const QString &message);

	QAction *previous = nullptr;
	QAction *playPause = nullptr;
	QAction *next = nullptr;

	QMenu *contextMenu = nullptr;

	QAction *currentTrack = nullptr;
	lib::spt::api &spotify;
	const lib::settings &settings;
	lib::cache &cache;
	const lib::http_client &httpClient;

#ifdef __APPLE__
	QAction *showApp = nullptr;
#endif

	std::function<void(const std::string &result)> callback;

	void onPrevious(bool checked);
	void onPlayPause(bool checked);
	void onNext(bool checked);
	void onActivated(ActivationReason reason);
	void onShowWindow(bool checked);
	void onMenuAboutToShow();

	void onPlaybackRefreshed(const lib::spt::playback &currentPlayback,
		const lib::spt::playback &previousPlayback);
};
