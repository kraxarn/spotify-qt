#include "view/context/content.hpp"

Context::Content::Content(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: QWidget(parent),
	spotify(spotify),
	current(current),
	cache(cache)
{
	auto *layout = new QHBoxLayout(this);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignBottom);

	album = new QLabel(this);
	album->setFixedSize(albumSize, albumSize);

	layout->addWidget(album);
	nowPlaying = new Context::NowPlaying(this);
	layout->addWidget(nowPlaying);

	reset();

	// Show menu when clicking
	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(this, &QWidget::customContextMenuRequested,
		this, &Context::Content::onSongMenu);

	// Context doesn't make sense to resize vertically
	setFixedHeight(layout->minimumSize().height());
}

void Context::Content::onSongMenu(const QPoint &pos)
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

void Context::Content::reset()
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

void Context::Content::setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage)
{
	if (album != nullptr)
	{
		album->setPixmap(Image::mask(albumImage));
		album->setToolTip(QString::fromStdString(albumEntity.name));
	}
}

auto Context::Content::getCurrentlyPlaying() const -> const lib::spt::track &
{
	return currentlyPlaying;
}

void Context::Content::setCurrentlyPlaying(const lib::spt::track &track)
{
	if (nowPlaying != nullptr)
	{
		nowPlaying->setTrack(track);
	}
	currentlyPlaying = track;
}

Context::Content::~Content()
{
	free(album);
	free(nowPlaying);
}
