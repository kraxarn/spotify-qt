#include "albummenu.hpp"

AlbumMenu::AlbumMenu(spt::Spotify &spotify, const QString &albumId, QWidget *parent)
	: parent(parent),
	albumId(albumId),
	spotify(spotify),
	QMenu(parent)
{
	auto mainWindow = MainWindow::find(parent);
	if (mainWindow == nullptr)
		return;

	tracks = mainWindow->loadTracksFromCache(albumId);
	if (tracks.isEmpty())
		tracks = spotify.albumTracks(albumId);

	auto duration = 0;
	for (auto &track : tracks)
		duration += track.duration;
	auto minutes = duration / 1000 / 60;
	if (tracks.length() > 1)
		addAction(QString("%1 tracks, %2%3 m")
			.arg(tracks.length())
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
	if (tracks.isEmpty())
	{
		((MainWindow *) parent)->setStatus("No tracks found to shuffle", true);
		return;
	}

	auto initialIndex = 0;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
	initialIndex = QRandomGenerator::global()->bounded(0, tracks.length());
#endif
	auto status = spotify.playTracks(initialIndex, QString("spotify:album:%1").arg(albumId));

	if (status.isEmpty())
		status = spotify.setShuffle(true);
	if (!status.isEmpty())
		((MainWindow *) parent)->setStatus(status, true);
}

void AlbumMenu::shareAlbum(bool)
{
	QApplication::clipboard()->setText(QString("https://open.spotify.com/album/%1")
		.arg(QString(albumId)));
	MainWindow::find(parentWidget())->setStatus("Link copied to clipboard");
}

void AlbumMenu::shareOpen(bool)
{
	Utils::openUrl(QString("https://open.spotify.com/album/%1")
		.arg(QString(albumId)), LinkType::Web, this->parent);
}
