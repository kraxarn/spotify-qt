#include "view/context/view.hpp"
#include "mainwindow.hpp"

Context::View::View(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: QDockWidget(parent)
{
	content = new Context::Content(spotify, current, cache, this);
	setWidget(content);

	title = new Context::Title(spotify, current, cache, this);
	setTitleBarWidget(title);

	setFeatures(QDockWidget::DockWidgetMovable | DockWidgetFloatable);
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
	if (content != nullptr)
	{
		content->reset();
	}
}

auto Context::View::getCurrentlyPlaying() const -> const lib::spt::track &
{
	if (content == nullptr)
	{
		throw std::runtime_error("No content");
	}
	return content->getCurrentlyPlaying();
}

void Context::View::setCurrentlyPlaying(const lib::spt::track &track) const
{
	if (content != nullptr)
	{
		content->setCurrentlyPlaying(track);
	}
}

void Context::View::setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage) const
{
	if (content != nullptr)
	{
		content->setAlbum(albumEntity, albumImage);
	}
}

void Context::View::setAlbum(const lib::spt::entity &albumEntity, const std::string &data) const
{
	if (content != nullptr)
	{
		content->setAlbumJpg(albumEntity, data);
	}
}
