#pragma once

#include "../util/icon.hpp"
#include "../util/utils.hpp"

#include <QSplashScreen>
#include <QPixmap>
#include <QPainter>

class SplashDialog: public QSplashScreen
{
Q_OBJECT

public:
	SplashDialog();
	void showMessage(const QString &message);
};