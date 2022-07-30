#pragma once

#include "util/icon.hpp"
#include "util/image.hpp"

#include <QSplashScreen>
#include <QPixmap>
#include <QPainter>

class SplashScreen: public QSplashScreen
{
Q_OBJECT

public:
	SplashScreen();

	void showMessage(const QString &message);

private:
	static constexpr int backgroundSize = 104;
	static constexpr int logoPos = 12;
	static constexpr int logoSize = 80;

	static constexpr QRgb backgroundColor = 0x4caf50;
	static constexpr QRgb foregroundColor = 0x212121;
};
