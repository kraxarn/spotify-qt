#include "view/context/albumcover.hpp"
using namespace lib;

Context::AlbumCover::AlbumCover(QWidget *parent)
	: QLabel(parent)
{
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
}

void Context::AlbumCover::setPixmap(const QPixmap &pixmap)
{
	cover = pixmap.copy();
	scaleCover(width(), width());
}

void Context::AlbumCover::scaleCover(int width, int height)
{
	auto pixmap = cover.scaled(width, height, Qt::KeepAspectRatioByExpanding);
	QLabel::setPixmap(pixmap);
}
