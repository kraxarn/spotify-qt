#include "view/splashscreen.hpp"

SplashScreen::SplashScreen()
{
	QPixmap background(backgroundSize, backgroundSize);
	background.fill(QColor::fromRgb(backgroundColor));

	QImage image(background.size(), QImage::Format_ARGB32);
	QPainter painter(&image);
	painter.drawPixmap(0, 0,
		Image::mask(background, MaskShape::App, QVariant()));
	painter.drawPixmap(logoPos, logoPos, Icon::get(QString("logo:%1")
		.arg(APP_ICON))
		.pixmap(logoSize, logoSize));

	setPixmap(QPixmap::fromImage(image));
}

void SplashScreen::showMessage(const QString &message)
{
	emit QSplashScreen::showMessage(message,
		Qt::AlignBottom, QColor::fromRgb(foregroundColor));
}
