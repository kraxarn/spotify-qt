#include "view/context/albumcover.hpp"
using namespace lib;

Context::AlbumCover::AlbumCover(QWidget *parent)
	: QLabel(parent)
{
	setFixedHeight(maxHeight);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
}

void Context::AlbumCover::setPixmap(const QPixmap &pixmap)
{
	cover = pixmap.copy();
	scaleCover(width());
}

void Context::AlbumCover::resizeEvent(QResizeEvent *event)
{
	lib::log::info("scale album cover requested?");
	if (cover.isNull())
	{
		lib::log::info("null?");
		QWidget::resizeEvent(event);
		return;
	}
	lib::log::info(std::to_string(event->size().width()));
	scaleCover(event->size().width());
}

void Context::AlbumCover::scaleCover(int width)
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
