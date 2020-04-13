#include "songmenu.hpp"

SongMenu::SongMenu(const QString &trackId, const QString &artist, const QString &name,
		const QString &artistId, const QString &albumId, spt::Spotify *spotify, QWidget *parent) : QMenu(parent)
{
	auto mainWindow = dynamic_cast<MainWindow*>(parent);
	if (mainWindow == nullptr)
		qDebug() << "warning: parent of song menu is not main window";

	auto trackFeatures = addAction(Icon::get("view-statistics"), "Audio features");
	QAction::connect(trackFeatures, &QAction::triggered, [=](bool checked) {
		mainWindow->openAudioFeaturesWidget(trackId, artist, name);
	});
	auto lyrics = addAction(Icon::get("view-media-lyrics"), "Lyrics");
	QAction::connect(lyrics, &QAction::triggered, [=](bool checked) {
		mainWindow->openLyrics(artist, name);
	});
	auto share = addMenu(Icon::get("document-share"), "Share");
	auto shareSongLink = share->addAction("Copy song link");
	QAction::connect(shareSongLink, &QAction::triggered, [=](bool checked) {
		QApplication::clipboard()->setText(
			QString("https://open.spotify.com/track/%1")
				.arg(QString(trackId).remove(0, QString("spotify:track:").length())));
		mainWindow->setStatus("Link copied to clipboard");
	});
	addSeparator();
	// Add to playlist
	auto addPlaylist = addMenu(Icon::get("list-add"), "Add to playlist");
	auto currentPlaylist = mainWindow->hasPlaylistSelected()
						   ? nullptr : &mainWindow->sptPlaylists->at(mainWindow->playlists->currentRow());
	for (auto &playlist : *mainWindow->sptPlaylists)
	{
		// Create main action
		auto action = addPlaylist->addAction(playlist.name);
		action->setData(playlist.id);
	}
	QMenu::connect(addPlaylist, &QMenu::triggered, [this, trackId, currentPlaylist, mainWindow, spotify](QAction *action) {
		// Check if it's already in the playlist
		auto playlistId = action->data().toString();
		auto tracks = mainWindow->playlistTracks(playlistId);
		for (auto &track : tracks)
		{
			if (trackId.endsWith(track.id))
			{
				if (QMessageBox::information(this,
					"Duplicate",
					"Track is already in the playlist, do you want to add it anyway?",
					QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)
					== QMessageBox::No)
					return;
				break;
			}
		}
		// Actually add
		auto result = spotify->addToPlaylist(playlistId, trackId);
		if (!result.isEmpty())
			mainWindow->setStatus(QString("Failed to add track to playlist: %1").arg(result));
	});
	// Remove from playlist
	auto remPlaylist = addAction(Icon::get("list-remove"), "Remove from playlist");
	QAction::connect(remPlaylist, &QAction::triggered, [this, trackId, name, artist, currentPlaylist, mainWindow, spotify](bool checked) {
		// Remove from interface
		QTreeWidgetItem *item = nullptr;
		int i;
		for (i = 0; i < mainWindow->songs->topLevelItemCount(); i++)
		{
			item = mainWindow->songs->topLevelItem(i);
			if (item->data(0, MainWindow::RoleTrackId).toString() == trackId)
				break;
			// Failed to find
			item = nullptr;
		}
		if (item == nullptr)
		{
			mainWindow->setStatus("Failed to remove track, not found in playlist");
			return;
		}
		// Remove from Spotify
		auto status = spotify->removeFromPlaylist(currentPlaylist->id, trackId,
			item->data(0, MainWindow::RoleIndex).toInt());
		// Update status
		if (!status.isEmpty())
		{
			mainWindow->setStatus(QString("Failed to remove track from playlist: %1").arg(status));
			return;
		}
		// i doesn't necessarily match item index depending on sorting order
		mainWindow->songs->takeTopLevelItem(i);
		mainWindow->setStatus(QString(R"(Removed "%1 - %2" from "%3")")
			.arg(name).arg(artist).arg(currentPlaylist->name));
	});
	addSeparator();
	auto goArtist = addAction(Icon::get("view-media-artist"), "View artist");
	QAction::connect(goArtist, &QAction::triggered, [=](bool checked) {
		mainWindow->openArtist(artistId);
	});
	auto goAlbum = addAction(Icon::get("view-media-album-cover"), "Open album");
	goAlbum->setEnabled(!mainWindow->sptContext.startsWith("spotify:album"));
	QAction::connect(goAlbum, &QAction::triggered, [=](bool checked) {
		mainWindow->loadAlbum(albumId, false);
	});
}