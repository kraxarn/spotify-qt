#include "view/context/view.hpp"
#include "mainwindow.hpp"

Context::View::View(lib::spt::api &spotify, lib::settings &settings, spt::Current &current,
	const lib::cache &cache, QWidget *parent)
	: QDockWidget(parent),
	spotify(spotify),
	current(current),
	cache(cache)
{
	setAlbumSize(settings.qt().album_size);

	title = new Context::Title(spotify, cache, this);
	setTitleBarWidget(title);

	setFeatures(QDockWidget::DockWidgetMovable | DockWidgetFloatable);

	auto *mainWindow = MainWindow::find(parent);

	MainWindow::connect(mainWindow, &MainWindow::playbackRefreshed,
		this, &Context::View::onPlaybackRefreshed);
}

void Context::View::setAlbumSize(lib::album_size albumSize)
{
	if (albumContent != nullptr)
	{
		albumContent->deleteLater();
	}

	if (albumSize == lib::album_size::expanded)
	{
		albumContent = new Context::ExpandedContent(spotify, cache, this);
	}
	else
	{
		albumContent = new Context::SmallContent(spotify, cache, this);
	}

	setWidget(albumContent);
}

void Context::View::setAlbum(const lib::spt::entity &albumEntity,
	const QPixmap &albumImage) const
{
	albumContent->setAlbum(albumEntity, albumImage);
}

void Context::View::onPlaybackRefreshed(const lib::spt::playback &playback)
{
	if (!playback.is_valid())
	{
		albumContent->reset();
		return;
	}

	albumContent->setCurrentlyPlaying(playback.item);
}
