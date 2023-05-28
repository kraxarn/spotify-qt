#include "view/context/abstractcontent.hpp"
#include "mainwindow.hpp"

Context::AbstractContent::AbstractContent(lib::spt::api &spotify, const lib::cache &cache,
	lib::settings &settings, QWidget *parent)
	: QWidget(parent),
	spotify(spotify),
	cache(cache),
	settings(settings)
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
		const auto albumShape = settings.qt().album_shape;
		album->setPixmap(Image::mask(albumImage, albumShape));
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
	isPlaying = false;

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
	isPlaying = true;

	if (nowPlaying != nullptr)
	{
		nowPlaying->setTrack(track);
	}
}

auto Context::AbstractContent::isCurrentlyPlaying() const -> bool
{
	return isPlaying;
}
