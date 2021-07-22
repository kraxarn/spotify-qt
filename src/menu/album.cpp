#include "menu/album.hpp"
#include "mainwindow.hpp"

Menu::Album::Album(lib::spt::api &spotify, lib::cache &cache, const std::string &albumId,
	QWidget *parent)
	: albumId(albumId),
	spotify(spotify),
	QMenu(parent)
{
	trackCount = addAction("...");
	trackCount->setEnabled(false);

	tracksLoaded(cache.get_tracks(albumId));
	spotify.album(albumId, [this](const lib::spt::album &album)
	{
		this->spotify.album_tracks(album, [this](const std::vector<lib::spt::track> &items)
		{
			this->tracksLoaded(items);
		});
	});

	addSeparator();
	auto *playShuffle = addAction(Icon::get("media-playlist-shuffle"), "Shuffle play");
	QAction::connect(playShuffle, &QAction::triggered, this, &Menu::Album::shuffle);

	auto *share = addMenu(Icon::get("document-share"), "Share");
	auto *sharePlaylist = share->addAction("Copy album link");
	QAction::connect(sharePlaylist, &QAction::triggered, this, &Menu::Album::shareAlbum);

	auto *shareSongOpen = share->addAction("Open in Spotify");
	QAction::connect(shareSongOpen, &QAction::triggered, this, &Menu::Album::shareOpen);
}

void Menu::Album::shuffle(bool /*checked*/)
{
	if (tracks.empty())
	{
		auto *mainWindow = MainWindow::find(parentWidget());
		mainWindow->setStatus("No tracks found to shuffle", true);
		return;
	}

	auto initialIndex = lib::random().next_int(0, tracks.size());
	spotify.play_tracks(initialIndex, lib::spt::api::to_uri("album", albumId),
		[this](const std::string &status)
		{
			auto *mainWindow = MainWindow::find(this->parentWidget());
			if (!status.empty())
			{
				mainWindow->status(status, true);
				return;
			}

			spotify.set_shuffle(true, [mainWindow](const std::string &status)
			{
				mainWindow->status(status, true);
			});
		});
}

void Menu::Album::shareAlbum(bool /*checked*/)
{
	QApplication::clipboard()->setText(QString("https://open.spotify.com/album/%1")
		.arg(QString::fromStdString(albumId)));
	MainWindow::find(parentWidget())->setStatus("Link copied to clipboard");
}

void Menu::Album::shareOpen(bool /*checked*/)
{
	UrlUtils::open(QString("https://open.spotify.com/album/%1")
			.arg(QString::fromStdString(albumId)), LinkType::Web,
		MainWindow::find(parentWidget()));
}

void Menu::Album::tracksLoaded(const std::vector<lib::spt::track> &items)
{
	tracks = items;

	auto duration = 0U;
	for (auto &track : tracks)
	{
		duration += track.duration;
	}

	constexpr unsigned int secInMin = 60U;
	constexpr unsigned int msToMin = 1000U * secInMin;
	auto minutes = duration / msToMin;

	trackCount->setText(QString("%1 tracks, %2%3 m")
		.arg(tracks.size())
		.arg(minutes >= secInMin
			? QString("%1 h ").arg(minutes / secInMin)
			: QString())
		.arg(minutes % secInMin));
}
