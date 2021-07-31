#include "view/context/context.hpp"
#include "mainwindow.hpp"

View::Context::Context::Context(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: QDockWidget(parent)
{
	content = new View::Context::Content(spotify, current, cache, this);
	setWidget(content);

	title = new View::Context::Title(spotify, current, cache, this);
	setTitleBarWidget(title);

	setFeatures(QDockWidget::DockWidgetMovable | DockWidgetFloatable);
}

void View::Context::Context::updateContextIcon()
{
	if (title != nullptr)
	{
		title->updateIcon();
	}
}

void View::Context::Context::resetCurrentlyPlaying() const
{
	if (content != nullptr)
	{
		content->reset();
	}
}

auto View::Context::Context::getCurrentlyPlaying() const -> const lib::spt::track &
{
	if (content == nullptr)
	{
		throw std::runtime_error("No content");
	}
	return content->getCurrentlyPlaying();
}

void View::Context::Context::setCurrentlyPlaying(const lib::spt::track &track) const
{
	if (content != nullptr)
	{
		content->setCurrentlyPlaying(track);
	}
}

void View::Context::Context::setAlbum(const QPixmap &pixmap) const
{
	if (content != nullptr)
	{
		content->setAlbum(pixmap);
	}
}
