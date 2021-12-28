#include "view/splashscreen.hpp"

SplashScreen::SplashScreen()
{
	QPixmap background(104, 104);
	background.fill(QColor::fromRgb(0x4caf50));

	QImage image(background.size(), QImage::Format_ARGB32);
	QPainter painter(&image);
	painter.drawPixmap(0, 0,
		Image::mask(background, MaskShape::App, QVariant()));
	painter.drawPixmap(12, 12, Icon::get(QString("logo:%1")
		.arg(APP_ICON))
		.pixmap(80, 80));

	setPixmap(QPixmap::fromImage(image));
}

void SplashScreen::showMessage(const QString &message)
{
	emit QSplashScreen::showMessage(message,
		Qt::AlignBottom, QColor::fromRgb(0x212121));
}
