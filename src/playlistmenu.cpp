#include "playlistmenu.hpp"

PlaylistMenu::PlaylistMenu(spt::Spotify &spotify, spt::Playlist playlist, QWidget *parent) : QMenu(parent)
{
	auto window = dynamic_cast<MainWindow*>(parent);
	if (window == nullptr)
		return;
	addAction(window->getImage("playlistImage", playlist.image), playlist.name)->setEnabled(false);
	if (!playlist.description.isEmpty())
		addAction(playlist.description)->setEnabled(false);

	auto tracks = window->playlistTracks(playlist.id);
	auto duration = 0;
	for (auto &track : tracks)
		duration += track.duration;
	auto minutes = duration / 1000 / 60;

	addAction(QString("%1 tracks, %2%3 m")
		.arg(tracks.length())
		.arg(minutes >= 60 ? QString("%1 h ").arg(minutes / 60) : QString())
		.arg(minutes % 60))->setEnabled(false);

	addSeparator();
	auto share = addMenu(Icon::get("document-share"), "Share");
	auto sharePlaylist = share->addAction("Copy playlist link");
	QAction::connect(sharePlaylist, &QAction::triggered, [window, playlist](bool checked) {
		QApplication::clipboard()->setText(
			QString("https://open.spotify.com/playlist/%1")
				.arg(QString(playlist.id)));
		window->setStatus("Link copied to clipboard");
	});
	auto shareSongOpen = share->addAction("Open in Spotify");
	QAction::connect(shareSongOpen, &QAction::triggered, [playlist](bool checked) {
		QDesktopServices::openUrl(QString("https://open.spotify.com/playlist/%1")
			.arg(QString(playlist.id)));
	});
}