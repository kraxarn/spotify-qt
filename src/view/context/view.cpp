#include "view/context/view.hpp"
#include "mainwindow.hpp"

Context::View::View(lib::spt::api &spotify, lib::settings &settings, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: QDockWidget(parent),
	spotify(spotify),
	current(current),
	cache(cache)
{
	// check if user wants a scalable album image or not
	albumShouldBeExpandable = settings.general.expand_album_cover;
	if (albumShouldBeExpandable)
	{
		horizContent = new Context::HorizContent(spotify, current, cache, this);
		setWidget(horizContent);
	}
	else 
	{
		content = new Context::Content(spotify, current, cache, this);
		setWidget(content);
	}

	title = new Context::Title(spotify, current, cache, this);
	setTitleBarWidget(title);

	setFeatures(QDockWidget::DockWidgetMovable | DockWidgetFloatable);
}

void Context::View::reloadAlbumContent(bool shouldBeExpandable)
{
	albumShouldBeExpandable = shouldBeExpandable;
	if (content == nullptr || horizContent == nullptr)
	{
		free(content);
		free(horizContent);
		content = nullptr;
		horizContent = nullptr;
	}	
	 
	if (albumShouldBeExpandable)
	{
		horizContent = new Context::HorizContent(spotify, current, cache, this);
		setWidget(horizContent);
	}
	else 
	{
		content = new Context::Content(spotify, current, cache, this);
		setWidget(content);
	}
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
	if (albumShouldBeExpandable && horizContent != nullptr)
	{
		horizContent->reset();
	}
	else if(!albumShouldBeExpandable && content != nullptr)
	{
		content->reset();
	}
}

auto Context::View::getCurrentlyPlaying() const -> const lib::spt::track &
{
	if (albumShouldBeExpandable && horizContent != nullptr)
	{
		return horizContent->getCurrentlyPlaying();
	}
	else if(!albumShouldBeExpandable && content != nullptr)
	{
		return content->getCurrentlyPlaying();
	}
	throw std::runtime_error("No content");
}

void Context::View::setCurrentlyPlaying(const lib::spt::track &track) const
{
	if (albumShouldBeExpandable && horizContent != nullptr)
	{
		horizContent->setCurrentlyPlaying(track);
	}
	else if(!albumShouldBeExpandable && content != nullptr)
	{
		content->setCurrentlyPlaying(track);
	}
}

void Context::View::setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage) const
{
	if (albumShouldBeExpandable && horizContent != nullptr)
	{
		horizContent->setAlbum(albumEntity, albumImage);
	}
	else if(!albumShouldBeExpandable && content != nullptr)
	{
		content->setAlbum(albumEntity, albumImage);
	}
}
