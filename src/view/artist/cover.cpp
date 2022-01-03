#include "view/artist/cover.hpp"

Artist::Cover::Cover(QWidget *parent)
	: QLabel(parent)
{
	setFixedHeight(maxHeight);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
}

void Artist::Cover::setJpeg(const QByteArray &jpeg)
{
	if (jpeg.isEmpty())
	{
		setFixedHeight(0);
		return;
	}

	cover.loadFromData(jpeg, "jpeg");
	scaleCover(width());
}

void Artist::Cover::resizeEvent(QResizeEvent *event)
{
	if (cover.isNull())
	{
		QWidget::resizeEvent(event);
		return;
	}
	scaleCover(event->size().width());
}

void Artist::Cover::scaleCover(int width)
{
	auto pixmap = cover.scaled(width, maxHeight,
		Qt::KeepAspectRatioByExpanding);

	const auto adjust = pixmap.height() - maxHeight;
	if (adjust > 0)
	{
		pixmap = pixmap.copy(0, adjust / 2,
			width, maxHeight);
	}

	setFixedHeight(pixmap.height());
	QLabel::setPixmap(pixmap);
}
