#include "view/context/content.hpp"

View::Context::Content::Content(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	current(current),
	cache(cache),
	QWidget(parent)
{
	auto *layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignBottom);

	album = new QLabel(this);
	album->setFixedSize(64, 64);

	layout->addWidget(album);
	nowPlaying = new View::Context::NowPlaying(this);
	layout->addWidget(nowPlaying);

	reset();

	// Show menu when clicking
	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(this, &QWidget::customContextMenuRequested,
		this, &View::Context::Content::onSongMenu);
}

void View::Context::Content::onSongMenu(const QPoint &pos)
{
	auto track = current.playback.item;
	if (track.name.empty()
		&& track.artists.empty())
	{
		return;
	}

	auto *menu = new SongMenu(track, spotify, cache, parentWidget());
	menu->popup(mapToGlobal(pos));
}

void View::Context::Content::reset()
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

void View::Context::Content::setAlbum(const QPixmap &pixmap)
{
	if (album != nullptr)
	{
		album->setPixmap(ImageUtils::mask(pixmap));
	}
}

auto View::Context::Content::getCurrentlyPlaying() const -> const lib::spt::track &
{
	return currentlyPlaying;
}

void View::Context::Content::setCurrentlyPlaying(const lib::spt::track &track)
{
	if (nowPlaying != nullptr)
	{
		nowPlaying->setTrack(track);
	}
	currentlyPlaying = track;
}
