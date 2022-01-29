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
	layout->setAlignment(Qt::AlignBottom);

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
		// auto scaledPixmap = scaleAlbum(300, albumImage);
		// auto = albumImage
		
		lib::log::info("this is a test");
		album->setPixmap(albumImage);
		// album->setPixmap(Image::mask(albumImage));
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

// QPixmap Context::HorizContent::scaleAlbum(int width, const QPixmap &albumImage)
// {
// 	lib::log::info("scaling");
// 	// auto pixmap = albumImage.scaled(width, width,
// 	// 	Qt::KeepAspectRatioByExpanding);
// 	auto pixmap = albumImage.scaled(width, width, Qt::KeepAspectRatio, Qt::SmoothTransformation);

// 	const auto adjust = pixmap.height() - width;
// 	lib::log::info(std::to_string(adjust));
// 	if (adjust > 0)
// 	{
// 		pixmap = pixmap.copy(0, adjust / 2,
// 			width, width);
// 	}

// 	return pixmap;

// 	// setFixedHeight(pixmap.height());
// 	// QLabel::setPixmap(pixmap);
// }

// void Context::HorizContent::resizeEvent(QResizeEvent *event)
// {
// 	// if (album.isNull())
// 	// {
// 	// 	QWidget::resizeEvent(event);
// 	// 	return;
// 	// }
// 	lib::log::info("resizing");

// 	const QPixmap* tempPixmap = album->pixmap();
// 	auto newSize = event->size().width();
// 	lib::log::info(std::to_string(newSize));
// 	auto scaledPixmap = scaleAlbum(newSize, *tempPixmap);
// 	album->setPixmap(scaledPixmap);
// }