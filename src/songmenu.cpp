#include "songmenu.hpp"

SongMenu::SongMenu(const QString &trackId, const QString &artist, const QString &name,
		const QString &artistId, const QString &albumId, spt::Spotify *spotify, QWidget *parent)
		: QMenu(parent)
{
	auto mainWindow = dynamic_cast<MainWindow*>(parent);
	if (mainWindow == nullptr)
		return;
	auto track = trackId.startsWith("spotify:track")
		? QString(trackId).remove(0, QString("spotify:track:").length())
		: trackId;
	auto trackFeatures = addAction(Icon::get("view-statistics"), "Audio features");
	QAction::connect(trackFeatures, &QAction::triggered, [mainWindow, trackId, artist, name](bool checked) {
		mainWindow->openAudioFeaturesWidget(trackId, artist, name);
	});
	auto lyrics = addAction(Icon::get("view-media-lyrics"), "Lyrics");
	QAction::connect(lyrics, &QAction::triggered, [mainWindow, artist, name](bool checked) {
		mainWindow->openLyrics(artist, name);
	});
	auto share = addMenu(Icon::get("document-share"), "Share");
	auto shareSongLink = share->addAction("Copy song link");
	QAction::connect(shareSongLink, &QAction::triggered, [track, mainWindow](bool checked) {
		QApplication::clipboard()->setText(
			QString("https://open.spotify.com/track/%1").arg(track));
		mainWindow->setStatus("Link copied to clipboard");
	});
	auto shareSongOpen = share->addAction("Open in Spotify");
	QAction::connect(shareSongOpen, &QAction::triggered, [track](bool checked) {
		QDesktopServices::openUrl(QString("https://open.spotify.com/track/%1").arg(track));
	});
	addSeparator();
	// Add/remove liked
	auto likedTracks = mainWindow->loadTracksFromCache("liked");
	auto isLiked = false;
	for (auto i = 0; i < likedTracks.length(); i++)
		if (likedTracks.at(i).id == track)
		{
			isLiked = true;
			break;
		}
	auto toggleLiked = addAction(
		Icon::get(isLiked ? "starred-symbolic" : "non-starred-symbolic"),
		isLiked ? "Dislike" : "Like");
	QAction::connect(toggleLiked, &QAction::triggered, [isLiked, spotify, trackId, mainWindow](bool checked) {
		auto status = isLiked
			? spotify->removeSavedTrack(trackId)
			: spotify->addSavedTrack(trackId);
		if (!status.isEmpty())
			mainWindow->setStatus(QString("Failed to %1: %2")
				.arg(isLiked ? "dislike" : "like")
				.arg(status), true);
	});
	// Add to queue
	auto addQueue = addAction(Icon::get("media-playlist-append"), "Add to queue");
	QAction::connect(addQueue, &QAction::triggered, [spotify, trackId, mainWindow](bool checked) {
		auto status = spotify->addToQueue(trackId.startsWith("spotify:track")
			? trackId
			: QString("spotify:track:%1").arg(trackId));
		if (!status.isEmpty())
			mainWindow->setStatus(status, true);
	});
	// Add to playlist
	addSeparator();
	auto addPlaylist = addMenu(Icon::get("list-add"), "Add to playlist");
	auto currentPlaylist = !mainWindow->hasPlaylistSelected()
		? nullptr
		: &mainWindow->getSptPlaylists().at(mainWindow->getPlaylistsList()->currentRow());
	for (auto &playlist : mainWindow->getSptPlaylists())
	{
		// Create main action
		auto action = addPlaylist->addAction(playlist.name);
		action->setData(playlist.id);
	}
	QMenu::connect(addPlaylist, &QMenu::triggered, [mainWindow, trackId, spotify](QAction *action) {
		// Check if it's already in the playlist
		auto playlistId = action->data().toString();
		auto tracks = mainWindow->playlistTracks(playlistId);
		for (auto &track : tracks)
		{
			if (trackId.endsWith(track.id))
			{
				if (QMessageBox::information(mainWindow,
					"Duplicate",
					"Track is already in the playlist, do you want to add it anyway?",
					QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)
					== QMessageBox::No)
					return;
				break;
			}
		}
		// Actually add
		auto plTrack = trackId.startsWith("spotify:track")
			? trackId
			: QString("spotify:track:%1").arg(trackId);
		auto result = spotify->addToPlaylist(playlistId, plTrack);
		if (!result.isEmpty())
			mainWindow->setStatus(QString("Failed to add track to playlist: %1").arg(result), true);
	});
	// Remove from playlist
	auto remPlaylist = addAction(Icon::get("list-remove"), "Remove from playlist");
	remPlaylist->setEnabled(mainWindow->getPlaylistsList()->currentRow() >= 0);
	QAction::connect(remPlaylist, &QAction::triggered, [
		mainWindow, trackId, spotify, currentPlaylist, name, artist](bool checked) {
		// Remove from interface
		QTreeWidgetItem *item = nullptr;
		int i;
		for (i = 0; i < mainWindow->getSongsTree()->topLevelItemCount(); i++)
		{
			item = mainWindow->getSongsTree()->topLevelItem(i);
			if (item->data(0, Utils::RoleTrackId).toString() == trackId)
				break;
			// Failed to find
			item = nullptr;
		}
		if (item == nullptr)
		{
			mainWindow->setStatus("Failed to remove track, not found in playlist", true);
			return;
		}
		// Remove from Spotify
		auto status = spotify->removeFromPlaylist(currentPlaylist->id, trackId,
			item->data(0, Utils::RoleIndex).toInt());
		// Update status
		if (!status.isEmpty())
		{
			mainWindow->setStatus(QString("Failed to remove track from playlist: %1").arg(status), true);
			return;
		}
		// i doesn't necessarily match item index depending on sorting order
		mainWindow->getSongsTree()->takeTopLevelItem(i);
		mainWindow->setStatus(QString(R"(Removed "%1 - %2" from "%3")")
			.arg(name).arg(artist).arg(currentPlaylist->name));
	});
	addSeparator();
	auto goArtist = addAction(Icon::get("view-media-artist"), "View artist");
	QAction::connect(goArtist, &QAction::triggered, [mainWindow, artistId](bool checked) {
		mainWindow->openArtist(artistId);
	});
	auto goAlbum = addAction(Icon::get("view-media-album-cover"), "Open album");
	QAction::connect(goAlbum, &QAction::triggered, [mainWindow, albumId](bool checked) {
		mainWindow->loadAlbum(albumId, false);
	});
}