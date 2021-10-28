#pragma once

#include "lib/spotify/api.hpp"
#include "util/icon.hpp"

#include <QMenu>
#include <QSystemTrayIcon>

class TrayIcon: public QSystemTrayIcon
{
Q_OBJECT

public:
	TrayIcon(lib::spt::api &spotify, const lib::settings &settings, QWidget *parent);
	~TrayIcon() override;

	void message(const QString &message);
	void message(const QString &message, const QPixmap &pixmap);

	void setPixmap(const QPixmap &pixmap);
	void setDefaultPixmap();

private:
	/**
	 * How long in milliseconds to show notifications with icons,
	 * for example when notifying about track changes
	 */
	static constexpr int messageIconTimeout = 6000;

	auto playback() -> lib::spt::playback;

	QAction *previous = nullptr;
	QAction *playPause = nullptr;
	QAction *next = nullptr;

	QMenu *contextMenu = nullptr;

	QAction *currentTrack = nullptr;
	lib::spt::api &spotify;
	const lib::settings &settings;

	std::function<void(const std::string &result)> callback;

	void onPrevious(bool checked);
	void onPlayPause(bool checked);
	void onNext(bool checked);
	void onActivated(ActivationReason reason);
	void onMenuAboutToShow();
};
