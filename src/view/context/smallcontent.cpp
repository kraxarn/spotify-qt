#include "view/context/smallcontent.hpp"
#include "view/context/abstractcontent.hpp"

Context::SmallContent::SmallContent(lib::spt::api &spotify,
	const lib::cache &cache, QWidget *parent)
	: AbstractContent(spotify, cache, parent)
{
	auto *layout = AbstractContent::layout<QHBoxLayout>();
	layout->setAlignment(Qt::AlignBottom);

	album = new QLabel(this);
	album->setFixedSize(albumSize, albumSize);

	layout->addWidget(album);
	nowPlaying = new Context::NowPlaying(this);
	layout->addWidget(nowPlaying);

	reset();

	// Context doesn't make sense to resize vertically
	setFixedHeight(layout->minimumSize().height());
}
