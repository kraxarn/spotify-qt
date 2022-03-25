#include "view/context/abstractcontent.hpp"

Context::AbstractContent::AbstractContent(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: QWidget(parent),
	spotify(spotify),
	current(current),
	cache(cache)
{
}

auto Context::AbstractContent::getCurrentlyPlaying() const -> const lib::spt::track &
{
	return currentlyPlaying;
}

void Context::AbstractContent::setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage)
{
	if (album != nullptr)
	{
		album->setPixmap(Image::mask(albumImage));
		album->setToolTip(QString::fromStdString(albumEntity.name));
	}
}

void Context::AbstractContent::onSongMenu(const QPoint &pos)
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

void Context::AbstractContent::reset()
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

void Context::AbstractContent::setCurrentlyPlaying(const lib::spt::track &track)
{
	if (nowPlaying != nullptr)
	{
		nowPlaying->setTrack(track);
	}
	currentlyPlaying = track;
}