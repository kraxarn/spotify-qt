#include "view/context/expandedcontent.hpp"
#include "view/context/abstractcontent.hpp"

Context::ExpandedContent::ExpandedContent(lib::spt::api &spotify,
	const lib::cache &cache, QWidget *parent)
	: AbstractContent(spotify, cache, parent)
{
	auto *layout = AbstractContent::layout<QGridLayout>();

	album = new QLabel(this);
	album->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	layout->addWidget(album, 0, 0, Qt::AlignBottom);

	nowPlaying = new Context::NowPlaying(this);
	nowPlaying->setTextShadow(true);
	layout->addWidget(nowPlaying, 0, 0, Qt::AlignBottom);

	setFixedHeight(width());
	reset();
}

void Context::ExpandedContent::resizeEvent(QResizeEvent *event)
{
	AbstractContent::resizeEvent(event);

	const auto size = qMin(event->size().width(), lib::spt::image::size_large);
	setFixedHeight(size);
	scaleAlbum(size);
}

void Context::ExpandedContent::scaleAlbum(int width)
{
	if (albumPixmap.isNull())
	{
		return;
	}

	const auto pixmap = albumPixmap.scaled(width, width);
	album->setPixmap(Image::mask(pixmap));
}

void Context::ExpandedContent::setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage)
{
	AbstractContent::setAlbum(albumEntity, albumImage);
	albumPixmap = albumImage;
	scaleAlbum(QWidget::width());
}
