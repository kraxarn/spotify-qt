#include "view/context/view.hpp"
#include "mainwindow.hpp"

Context::View::View(lib::spt::api &spotify, lib::settings &settings, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: QDockWidget(parent),
	spotify(spotify),
	settings(settings),
	cache(cache),
	httpClient(httpClient)
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
		albumContent = new Context::ExpandedContent(spotify, cache, settings, this);
	}
	else
	{
		albumContent = new Context::SmallContent(spotify, cache, settings, this);
	}

	setWidget(albumContent);
}

void Context::View::setAlbumImage(const lib::spt::entity &albumEntity,
	const std::string &albumImageUrl)
{
	Http::getAlbumImage(albumImageUrl, httpClient, cache, [this, albumEntity](const QPixmap &image)
	{
		albumContent->setAlbum(albumEntity, image);
	});
}

void Context::View::reset()
{
	albumContent->reset();
}

void Context::View::onPlaybackRefreshed(const lib::spt::playback &current,
	const lib::spt::playback &previous)
{
	if (!current.is_valid())
	{
		reset();
		return;
	}

	if (current.item.id != previous.item.id
		|| !albumContent->isCurrentlyPlaying()
		|| MainWindow::find(parent())->windowTitle() == QStringLiteral(APP_NAME))
	{
		const auto &albumImageUrl = settings.qt().album_size == lib::album_size::expanded
			? current.item.image_large()
			: current.item.image_small();

		setAlbumImage(current.item.album, albumImageUrl);
	}

	albumContent->setCurrentlyPlaying(current.item);
}
