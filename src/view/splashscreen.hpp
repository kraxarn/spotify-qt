#pragma once

#include "util/icon.hpp"
#include "util/imageutils.hpp"

#include <QSplashScreen>
#include <QPixmap>
#include <QPainter>

class SplashScreen: public QSplashScreen
{
Q_OBJECT

public:
	SplashScreen();

	void showMessage(const QString &message);
};
