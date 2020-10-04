#include "songmenu.hpp"
#include <utility>

SongMenu::SongMenu(QTreeWidgetItem *item, spt::Spotify &spotify, QWidget *parent)
	: SongMenu(item->data(0, RoleTrackId).toString(), item->text(2),
	item->text(1), item->data(0, RoleArtistId).toString(),
	item->data(0, RoleAlbumId).toString(),
	item->data(0, RoleIndex).toInt(), spotify, parent)
{
}

SongMenu::SongMenu(QListWidgetItem *item, QString artist, spt::Spotify &spotify, QWidget *parent)
	: SongMenu(item->data(RoleTrackId).toString(), std::move(artist),
	item->text(), item->data(RoleArtistId).toString(),
	item->data(RoleAlbumId).toString(),
	item->data(RoleIndex).toInt(), spotify, parent)
{
}

SongMenu::SongMenu(const spt::Track &track, spt::Spotify &spotify, QWidget *parent)
	: SongMenu(track.id, track.artist, track.name, track.artistId, track.albumId, 0, spotify, parent)
{
}

SongMenu::SongMenu(const QString &trackId, QString artist, QString name, QString artistId,
	QString albumId, int index, spt::Spotify &spotify, QWidget *parent)
	: trackId(trackId), artist(std::move(artist)), trackName(std::move(name)), index(index), spotify(spotify),
	artistId(std::move(artistId)), albumId(std::move(albumId)), QMenu(parent)
{
	this->parent = dynamic_cast<MainWindow *>(parent);
	if (this->parent == nullptr)
	{
		qDebug() << "warning: parent is not main window, song menu won't function";
		return;
	}
	auto mainWindow = (MainWindow *) parent;

	trackUri = trackId.startsWith("spotify:track")
		? QString(trackId).remove(0, QString("spotify:track:").length())
		: trackId;
	auto trackFeatures = addAction(Icon::get("view-statistics"), "Audio features");
	QAction::connect(trackFeatures, &QAction::triggered, this, &SongMenu::openTrackFeatures);

	auto lyrics = addAction(Icon::get("view-media-lyrics"), "Lyrics");
	QAction::connect(lyrics, &QAction::triggered, this, &SongMenu::openLyrics);

	auto share = addMenu(Icon::get("document-share"), "Share");
	auto shareSongLink = share->addAction("Copy song link");
	QAction::connect(shareSongLink, &QAction::triggered, [this](bool checked)
	{
		QApplication::clipboard()->setText(QString("https://open.spotify.com/track/%1").arg(trackUri));
		((MainWindow *) this->parent)->setStatus("Link copied to clipboard");
	});

	auto shareSongOpen = share->addAction("Open in Spotify");
	QAction::connect(shareSongOpen, &QAction::triggered, [this](bool checked)
	{
		Utils::openUrl(QString("https://open.spotify.com/track/%1").arg(trackUri),
			LinkType::Web, this->parent);
	});

	// Add/remove liked
	addSeparator();
	auto likedTracks = mainWindow->loadTracksFromCache("liked");
	isLiked = false;
	for (const auto &likedTrack : likedTracks)
		if (likedTrack.id == trackUri)
		{
			isLiked = true;
			break;
		}

	auto toggleLiked = addAction(Icon::get(isLiked ? "starred-symbolic" : "non-starred-symbolic"),
		isLiked ? "Dislike" : "Like");
	QAction::connect(toggleLiked, &QAction::triggered, this, &SongMenu::like);

	// Add to queue
	auto addQueue = addAction(Icon::get("media-playlist-append"), "Add to queue");
	QAction::connect(addQueue, &QAction::triggered, this, &SongMenu::addToQueue);

	// Add to playlist
	addSeparator();
	auto addPlaylist = addMenu(Icon::get("list-add"), "Add to playlist");

	auto playlistItem = mainWindow->getPlaylistsList()->currentItem();
	if (playlistItem != nullptr)
	{
		currentPlaylist = &mainWindow->getSptPlaylists().at(playlistItem->data(RoleIndex).toInt());
	}

	auto currentUserId = mainWindow->getCurrentUser().id;
	for (auto &playlist : mainWindow->getSptPlaylists())
	{
		if (!playlist.collaborative && playlist.ownerId != currentUserId)
			continue;

		// Create main action
		auto action = addPlaylist->addAction(playlist.name);
		action->setData(playlist.id);
	}
	QMenu::connect(addPlaylist, &QMenu::triggered, this, &SongMenu::addToPlaylist);

	// Remove from playlist
	auto remPlaylist = addAction(Icon::get("list-remove"), "Remove from playlist");
	remPlaylist->setVisible(mainWindow->getPlaylistsList()->currentRow() >= 0);
	QAction::connect(remPlaylist, &QAction::triggered, this, &SongMenu::remFromPlaylist);

	addSeparator();
	auto goArtist = addAction(Icon::get("view-media-artist"), "View artist");
	QAction::connect(goArtist, &QAction::triggered, this, &SongMenu::viewArtist);

	auto goAlbum = addAction(Icon::get("view-media-album-cover"), "Open album");
	goAlbum->setVisible(this->albumId.length() > 1);
	QAction::connect(goAlbum, &QAction::triggered, this, &SongMenu::openAlbum);
}

void SongMenu::like(bool)
{
	auto status = isLiked
		? spotify.removeSavedTrack(trackId)
		: spotify.addSavedTrack(trackId);
	if (!status.isEmpty())
		((MainWindow *) parent)->setStatus(QString("Failed to %1: %2")
			.arg(isLiked ? "dislike" : "like")
			.arg(status), true);
}

void SongMenu::addToQueue(bool)
{
	auto status = spotify.addToQueue(trackId.startsWith("spotify:track")
		? trackId
		: QString("spotify:track:%1").arg(trackId));
	if (!status.isEmpty())
		((MainWindow *) parent)->setStatus(status, true);
}

void SongMenu::addToPlaylist(QAction *action)
{
	// Check if it's already in the playlist
	auto mainWindow = (MainWindow *) parent;
	auto playlistId = action->data().toString();
	auto tracks = spotify.playlist(playlistId).loadTracks(spotify);
	for (auto &item : tracks)
	{
		if (trackId.endsWith(item.id))
		{
			if (QMessageBox::information(mainWindow, "Duplicate",
				"Track is already in the playlist, do you want to add it anyway?",
				QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
				return;
			break;
		}
	}

	// Actually add
	auto plTrack = trackId.startsWith("spotify:track")
		? trackId
		: QString("spotify:track:%1").arg(trackId);
	auto result = spotify.addToPlaylist(playlistId, plTrack);
	if (!result.isEmpty())
		mainWindow->setStatus(QString("Failed to add track to playlist: %1").arg(result), true);
}

void SongMenu::remFromPlaylist(bool)
{
	// Remove from Spotify
	auto mainWindow = (MainWindow *) parent;
	auto status = spotify.removeFromPlaylist(currentPlaylist->id, trackId, index);
	if (!status.isEmpty())
	{
		mainWindow->setStatus(QString("Failed to remove track from playlist: %1").arg(status), true);
		return;
	}

	// Remove from interface
	QTreeWidgetItem *item = nullptr;
	int i;
	for (i = 0; i < mainWindow->getSongsTree()->topLevelItemCount(); i++)
	{
		item = mainWindow->getSongsTree()->topLevelItem(i);
		if (item->data(0, RoleTrackId).toString() == trackId)
			break;
		item = nullptr;
	}

	if (item == nullptr)
	{
		mainWindow->setStatus("Failed to remove track, not found in playlist", true);
		return;
	}

	// i doesn't necessarily match item index depending on sorting order
	mainWindow->getSongsTree()->takeTopLevelItem(i);
	mainWindow->setStatus(QString(R"(Removed "%1 - %2" from "%3")")
		.arg(trackName).arg(artist).arg(currentPlaylist->name));
}

void SongMenu::openTrackFeatures(bool)
{
	((MainWindow *) parent)->openAudioFeaturesWidget(trackId, artist, trackName);
}

void SongMenu::openLyrics(bool)
{
	((MainWindow *) parent)->openLyrics(artist, trackName);
}

void SongMenu::viewArtist(bool)
{
	((MainWindow *) parent)->openArtist(artistId);
}

void SongMenu::openAlbum(bool)
{
	((MainWindow *) parent)->loadAlbum(albumId, false);
}
