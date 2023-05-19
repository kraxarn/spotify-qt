#include "view/context/expandedcontent.hpp"
#include "view/context/abstractcontent.hpp"

Context::ExpandedContent::ExpandedContent(lib::spt::api &spotify,
	const lib::cache &cache, QWidget *parent)
	: AbstractContent(spotify, cache, parent)
{
	auto *layout = AbstractContent::layout<QGridLayout>();

	album = new QLabel(this);
	album->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	layout->addWidget(album, 0, 0, Qt::AlignCenter);

	nowPlaying = new Context::NowPlaying(this);
	nowPlaying->setTextShadow(true);
	layout->addWidget(nowPlaying, 0, 0, Qt::AlignBottom);

	setMaximumWidth(lib::spt::image::size_large);
	setFixedHeight(width());
	reset();
}

void Context::ExpandedContent::resizeEvent(QResizeEvent *event)
{
	AbstractContent::resizeEvent(event);

	const auto size = event->size().width();
	setFixedHeight(size);
	scaleAlbum(size);
}

auto Context::ExpandedContent::iconSize() const -> QSize
{
	return {100, 100};
}

void Context::ExpandedContent::scaleAlbum(int width)
{
	if (albumPixmap.isNull())
	{
		return;
	}

	const auto contentMargins = QWidget::layout()->contentsMargins();
	const auto size = width - contentMargins.left() - contentMargins.right();

	const auto pixmap = albumPixmap.scaled(size, size,
		Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

	album->setPixmap(Image::mask(pixmap));
}

void Context::ExpandedContent::setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage)
{
	AbstractContent::setAlbum(albumEntity, albumImage);
	albumPixmap = albumImage;
	scaleAlbum(QWidget::width());
}
