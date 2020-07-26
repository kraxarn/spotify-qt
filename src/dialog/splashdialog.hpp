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
	explicit SplashDialog(const QPalette &palette);
	void showMessage(const QString &message);

private:
	const QPalette &palette;
};