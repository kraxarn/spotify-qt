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
};
