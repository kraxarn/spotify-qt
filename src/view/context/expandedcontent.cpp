#include "view/context/expandedcontent.hpp"
#include "view/context/abstractcontent.hpp"

Context::ExpandedContent::ExpandedContent(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: AbstractContent(spotify, current, cache, parent)
{
	auto *layout = new QBoxLayout(QVBoxLayout::Direction::BottomToTop, this);
	layout->setSpacing(0);

	album = new AlbumCover(this);

	layout->addWidget(album);
	nowPlaying = new Context::NowPlaying(this);
	layout->addWidget(nowPlaying);

	AbstractContent::reset();

	// Show menu when clicking
	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(this, &QWidget::customContextMenuRequested,
		this, &Context::ExpandedContent::onSongMenu);
}

void Context::ExpandedContent::resizeEvent(QResizeEvent *event)
{
	auto newWidth = event->size().width();
	auto newHeight = event->size().height();

	QWidget::setFixedHeight(newWidth);
	album->scaleCover(newWidth, newHeight);
}
