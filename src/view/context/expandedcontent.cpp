#include "view/context/expandedcontent.hpp"
#include "view/context/abstractcontent.hpp"

Context::ExpandedContent::ExpandedContent(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: AbstractContent(spotify, current, cache, parent)
{
	auto *layout = AbstractContent::layout<QHBoxLayout>();

	album = new AlbumCover(this);
	layout->addWidget(album);

	nowPlaying = new Context::NowPlaying(this);
	layout->addWidget(nowPlaying);

	reset();
}

void Context::ExpandedContent::resizeEvent(QResizeEvent *event)
{
	auto newWidth = event->size().width();
	auto newHeight = event->size().height();

	QWidget::setFixedHeight(newWidth);
	album->scaleCover(newWidth, newHeight);
}
