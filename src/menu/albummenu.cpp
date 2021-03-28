#include "albummenu.hpp"

AlbumMenu::AlbumMenu(spt::Spotify &spotify, const std::string &albumId, QWidget *parent)
	: parent(parent),
	albumId(albumId),
	spotify(spotify),
	QMenu(parent)
{
	auto mainWindow = MainWindow::find(parent);
	if (mainWindow == nullptr)
		return;

	tracks = mainWindow->loadTracksFromCache(albumId);
	if (tracks.empty())
		tracks = spotify.albumTracks(albumId);

	auto duration = 0;
	for (auto &track : tracks)
		duration += track.duration;
	auto minutes = duration / 1000 / 60;
	if (tracks.size() > 1)
		addAction(QString("%1 tracks, %2%3 m")
			.arg(tracks.size())
			.arg(minutes >= 60 ? QString("%1 h ").arg(minutes / 60) : QString())
			.arg(minutes % 60))->setEnabled(false);

	addSeparator();
	auto playShuffle = addAction(Icon::get("media-playlist-shuffle"), "Shuffle play");
	QAction::connect(playShuffle, &QAction::triggered, this, &AlbumMenu::shuffle);

	auto share = addMenu(Icon::get("document-share"), "Share");
	auto sharePlaylist = share->addAction("Copy album link");
	QAction::connect(sharePlaylist, &QAction::triggered, this, &AlbumMenu::shareAlbum);

	auto shareSongOpen = share->addAction("Open in Spotify");
	QAction::connect(shareSongOpen, &QAction::triggered, this, &AlbumMenu::shareOpen);
}

void AlbumMenu::shuffle(bool)
{
	if (tracks.empty())
	{
		((MainWindow *) parent)->setStatus("No tracks found to shuffle", true);
		return;
	}

	auto initialIndex = lib::random().next_int(0, tracks.size());
	spotify.play_tracks(initialIndex, lib::fmt::format("spotify:album:{}", albumId),
		[this](const std::string &status)
		{
			auto mainWindow = MainWindow::find(this->parent);
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

void AlbumMenu::shareAlbum(bool)
{
	QApplication::clipboard()->setText(QString("https://open.spotify.com/album/%1")
		.arg(QString::fromStdString(albumId)));
	MainWindow::find(parentWidget())->setStatus("Link copied to clipboard");
}

void AlbumMenu::shareOpen(bool)
{
	Utils::openUrl(QString("https://open.spotify.com/album/%1")
		.arg(QString::fromStdString(albumId)), LinkType::Web, this->parent);
}
