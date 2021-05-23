#include "splashdialog.hpp"

SplashDialog::SplashDialog()
	: QSplashScreen()
{
	QPixmap background(104, 104);
	background.fill(QColor::fromRgb(0x4caf50));

	QImage image(background.size(), QImage::Format_ARGB32);
	QPainter painter(&image);
	painter.drawPixmap(0, 0,
		ImageUtils::mask(background, MaskShape::App, QVariant()));
	painter.drawPixmap(12, 12, Icon::get("logo:spotify-qt").pixmap(80, 80));

	setPixmap(QPixmap::fromImage(image));
}

void SplashDialog::showMessage(const QString &message)
{
	emit QSplashScreen::showMessage(message,
		Qt::AlignBottom, QColor::fromRgb(0x212121));
}
