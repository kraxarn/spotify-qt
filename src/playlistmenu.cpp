#include "playlistmenu.hpp"

PlaylistMenu::PlaylistMenu(spt::Spotify &spotify, const spt::Playlist &playlist, QWidget *parent) : QMenu(parent)
{
	auto window = dynamic_cast<MainWindow*>(parent);
	if (window == nullptr)
		return;
	auto isOwner = !playlist.ownerId.isEmpty() && playlist.ownerId == window->getCurrentUser().id;
	auto playlistName = addAction(window->getImage("playlistImage", playlist.image), playlist.name);
	playlistName->setEnabled(isOwner);
	QAction::connect(playlistName, &QAction::triggered, [window, playlist](bool checked) {
		QInputDialog::getText(window, "New name",
			"Name:", QLineEdit::Normal, playlist.name);
	});
	QAction *editDescription = nullptr;
	if (!playlist.description.isEmpty())
		editDescription = addAction(playlist.description);

	auto tracks = window->playlistTracks(playlist.id);
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
	QAction::connect(playShuffle, &QAction::triggered, [tracks, playlist, &spotify, window](bool checked) {
		auto initial = tracks.at(QRandomGenerator::global()->bounded(tracks.length()));
		auto status = spotify.playTracks(
			QString("spotify:track:%1").arg(initial.id),
			QString("spotify:playlist:%1").arg(playlist.id));
		if (status.isEmpty())
			status = spotify.setShuffle(true);
		if (!status.isEmpty())
			window->setStatus(status, true);
	});

	auto editPlaylist = addMenu(Icon::get("document-edit"), "Edit");
	if (editDescription == nullptr)
		editDescription = editPlaylist->addAction("Add description");
	editDescription->setEnabled(isOwner);
	QAction::connect(editDescription, &QAction::triggered, [window, playlist](bool checked) {
		auto text = TextEditDialog::getText("New description",
			"Description:", playlist.description, window);
		qDebug() << text;
	});
	auto editPublic = editPlaylist->addAction("Public");
	editPublic->setCheckable(true);
	editPublic->setChecked(playlist.isPublic);
	auto editCollaborative = editPlaylist->addAction("Collaborative");
	editCollaborative->setCheckable(true);
	editCollaborative->setChecked(playlist.collaborative);

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