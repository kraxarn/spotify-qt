#pragma once

#include "util/icon.hpp"
#include "util/image.hpp"
#include "spotify/spotify.hpp"

#include <QSplashScreen>
#include <QPixmap>
#include <QPainter>

class SplashScreen: public QSplashScreen
{
Q_OBJECT

public:
	SplashScreen(lib::settings &settings, spt::Spotify &spotify, QWidget *parent);

signals:
	void success();
	void failed();

protected:
	void showEvent(QShowEvent *event) override;

private:
	static constexpr int backgroundSize = 104;
	static constexpr int logoPos = 12;
	static constexpr int logoSize = 80;

	static constexpr QRgb backgroundColor = 0x4caf50;
	static constexpr QRgb foregroundColor = 0x212121;

	static constexpr int closeTimer = 100;

	/**
	 * Used for waiting for parent to close before closing splash
	 * (constructor with QWidget is deprecated)
	 */
	QWidget *parent;

	lib::settings &settings;
	spt::Spotify &spotify;

	void showMessage(const QString &message);
	void close(bool successful, bool wait = false);
};
