#include "view/artistcover.hpp"

View::ArtistCover::ArtistCover(QWidget *parent)
	: QLabel(parent)
{
	setFixedHeight(maxHeight);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
}

void View::ArtistCover::setPixmap(const QByteArray &jpeg)
{
	cover.loadFromData(jpeg, "jpeg");
	scaleCover(maxWidth);
}

void View::ArtistCover::resizeEvent(QResizeEvent *event)
{
	if (cover.isNull())
	{
		QWidget::resizeEvent(event);
		return;
	}
	scaleCover(event->size().width());
}

void View::ArtistCover::scaleCover(int width)
{
	auto pixmap = cover
		.copy(0, offset, maxWidth, maxHeight)
		.scaled(width, maxHeight, Qt::KeepAspectRatio);

	setFixedHeight(pixmap.height());
	QLabel::setPixmap(pixmap);
}
