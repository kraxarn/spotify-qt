#include "view/context/horizcontent.hpp"

Context::HorizContent::HorizContent(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: QWidget(parent),
	spotify(spotify),
	current(current),
	cache(cache)
{
	auto *layout = new QBoxLayout(QVBoxLayout::Direction::BottomToTop, this);
	layout->setSpacing(0);

	album = new AlbumCover(this);

	layout->addWidget(album);
	nowPlaying = new Context::NowPlaying(this);
	layout->addWidget(nowPlaying);

	reset();

	// Show menu when clicking
	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(this, &QWidget::customContextMenuRequested,
		this, &Context::HorizContent::onSongMenu);
}

void Context::HorizContent::onSongMenu(const QPoint &pos)
{
	auto track = current.playback.item;
	if (track.name.empty()
		&& track.artists.empty())
	{
		return;
	}

	auto *menu = new Menu::Track(track, spotify, cache, parentWidget());
	menu->popup(mapToGlobal(pos));
}

void Context::HorizContent::reset()
{
	if (album != nullptr)
	{
		album->setPixmap(Icon::get("media-optical-audio").pixmap(album->size()));
	}

	if (nowPlaying != nullptr)
	{
		nowPlaying->setNoPlaying();
	}
}

void Context::HorizContent::setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage)
{
	if (album != nullptr)
	{
		album->setPixmap(albumImage);
		album->setToolTip(QString::fromStdString(albumEntity.name));
	}
}

auto Context::HorizContent::getCurrentlyPlaying() const -> const lib::spt::track &
{
	return currentlyPlaying;
}

void Context::HorizContent::setCurrentlyPlaying(const lib::spt::track &track)
{
	if (nowPlaying != nullptr)
	{
		nowPlaying->setTrack(track);
	}
	currentlyPlaying = track;
}

void Context::HorizContent::resizeEvent(QResizeEvent *event)
{
	auto newWidth = event->size().width();
	auto newHeight = event->size().height();
	
	QWidget::setFixedHeight(newWidth);
	album->scaleCover(newWidth, newHeight);
}