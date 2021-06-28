#include "view/context.hpp"

#include "mainwindow.hpp"

View::Context::Context(lib::spt::api &spotify, const lib::settings &settings,
	spt::Current &current, const lib::cache &cache, QWidget *parent)
	: QDockWidget(parent)
{
	content = new View::ContextContent(spotify, settings, current, cache, this);
	setWidget(content);

	title = new View::ContextTitle(spotify, settings, current, cache, this);
	setTitleBarWidget(title);
}

void View::Context::updateContextIcon()
{
	if (title != nullptr)
	{
		title->updateIcon();
	}
}

void View::Context::resetCurrentlyPlaying()
{
	if (content != nullptr)
	{
		content->reset();
	}
}

auto View::Context::getCurrentlyPlaying() const -> const lib::spt::track &
{
	if (content == nullptr)
	{
		throw std::runtime_error("No content");
	}
	return content->getCurrentlyPlaying();
}

void View::Context::setCurrentlyPlaying(const lib::spt::track &track)
{
	if (content != nullptr)
	{
		content->setCurrentlyPlaying(track);
	}
}

void View::Context::setAlbum(const QPixmap &pixmap)
{
	if (content != nullptr)
	{
		content->setAlbum(pixmap);
	}
}
