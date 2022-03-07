#include "view/context/content.hpp"
#include "view/context/abstractcontent.hpp"

Context::Content::Content(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: AbstractContent(spotify, current, cache, parent)
{
	auto *layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignBottom);

	album = new QLabel(this);
	album->setFixedSize(albumSize, albumSize);

	layout->addWidget(album);
	nowPlaying = new Context::NowPlaying(this);
	layout->addWidget(nowPlaying);

	AbstractContent::reset();

	// Show menu when clicking
	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(this, &QWidget::customContextMenuRequested,
		this, &Context::Content::onSongMenu);

	// Context doesn't make sense to resize vertically
	setFixedHeight(layout->minimumSize().height());
}

void Context::Content::setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage)
{
	if (album != nullptr)
	{
		album->setPixmap(Image::mask(albumImage));
		album->setToolTip(QString::fromStdString(albumEntity.name));
	}
}