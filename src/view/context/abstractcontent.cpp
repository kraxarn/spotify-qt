#include "view/context/abstractcontent.hpp"
#include "mainwindow.hpp"

Context::AbstractContent::AbstractContent(lib::spt::api &spotify,
	const lib::cache &cache, QWidget *parent)
	: QWidget(parent),
	spotify(spotify),
	cache(cache)
{
	// Show menu when clicking
	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(this, &QWidget::customContextMenuRequested,
		this, &Context::AbstractContent::onSongMenu);
}

void Context::AbstractContent::setAlbum(const lib::spt::entity &albumEntity,
	const QPixmap &albumImage)
{
	if (album != nullptr)
	{
		album->setPixmap(Image::mask(albumImage));
		album->setToolTip(QString::fromStdString(albumEntity.name));
	}
}

void Context::AbstractContent::onSongMenu(const QPoint &pos)
{
	auto *mainWindow = MainWindow::find(parent());
	if (mainWindow == nullptr)
	{
		return;
	}

	auto track = mainWindow->playback().item;
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
		album->setPixmap(Icon::get("media-optical-audio").pixmap(iconSize()));
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
}
