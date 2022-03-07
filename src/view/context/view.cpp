#include "view/context/view.hpp"
#include "mainwindow.hpp"

Context::View::View(lib::spt::api &spotify, lib::settings &settings, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: QDockWidget(parent),
	spotify(spotify),
	current(current),
	cache(cache)
{
	reloadAlbumContent(settings.general.expand_album_cover);

	title = new Context::Title(spotify, current, cache, this);
	setTitleBarWidget(title);

	setFeatures(QDockWidget::DockWidgetMovable | DockWidgetFloatable);
}

void Context::View::reloadAlbumContent(bool shouldBeExpandable)
{
	albumShouldBeExpandable = shouldBeExpandable;
	 
	// check if user wants a scalable album image or not
	if (albumShouldBeExpandable)
	{
		albumContent = new Context::HorizContent(spotify, current, cache, this);
	}
	else 
	{
		albumContent = new Context::Content(spotify, current, cache, this);
	}
	setWidget(albumContent);
}

void Context::View::updateContextIcon()
{
	if (title != nullptr)
	{
		title->updateIcon();
	}
}

void Context::View::resetCurrentlyPlaying() const
{
	albumContent->reset();
}

auto Context::View::getCurrentlyPlaying() const -> const lib::spt::track &
{
	return albumContent->getCurrentlyPlaying();
}

void Context::View::setCurrentlyPlaying(const lib::spt::track &track) const
{
	albumContent->setCurrentlyPlaying(track);
}

void Context::View::setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage) const
{
	albumContent->setAlbum(albumEntity, albumImage);
}
