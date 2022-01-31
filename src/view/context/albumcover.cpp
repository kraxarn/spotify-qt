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
	scaleCover(width(), width());
}

// void Context::AlbumCover::resizeEvent(QResizeEvent *event)
// {
// 	// lib::log::info("scale album cover requested?");
// 	if (cover.isNull())
// 	{
// 		lib::log::info("null?");
// 		QWidget::resizeEvent(event);
// 		return;
// 	}

// 	auto newWidth = event->size().width();
// 	auto newHeight = event->size().height();
// 	lib::log::info("width and height: " + std::to_string(newWidth) + ", " + std::to_string(newHeight));
// 	scaleCover(event->size().width());
// }

void Context::AlbumCover::scaleCover(int width, int height)
{
	// auto pixmap = cover.scaled(width, maxHeight, Qt::KeepAspectRatioByExpanding);
	auto pixmap = cover.scaledToWidth(width-10, Qt::SmoothTransformation);
	lib::log::info("new pixmap height?" + std::to_string(pixmap.height()));
	cover = pixmap;

	// const auto adjust = pixmap.height() - maxHeight;
	// if (adjust > 0)
	// {
	// 	pixmap = pixmap.copy(0, adjust / 2,
	// 		width, maxHeight);
	// }

	// setFixedHeight(pixmap.height());
	QLabel::setPixmap(cover);
}
