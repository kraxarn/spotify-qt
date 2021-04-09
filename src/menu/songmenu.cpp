#include "songmenu.hpp"

#include "mainwindow.hpp"

SongMenu::SongMenu(QTreeWidgetItem *item, spt::Spotify &spotify, QWidget *parent)
	: SongMenu(item->data(0, RoleTrackId).toString().toStdString(),
	item->text(2).toStdString(), item->text(1).toStdString(),
	item->data(0, RoleArtistId).toString().toStdString(),
	item->data(0, RoleAlbumId).toString().toStdString(),
	item->data(0, RoleIndex).toInt(), spotify, parent)
{
}

SongMenu::SongMenu(QListWidgetItem *item, std::string artist, spt::Spotify &spotify,
	QWidget *parent)
	: SongMenu(item->data(RoleTrackId).toString().toStdString(), std::move(artist),
	item->text().toStdString(), item->data(RoleArtistId).toString().toStdString(),
	item->data(RoleAlbumId).toString().toStdString(),
	item->data(RoleIndex).toInt(), spotify, parent)
{
}

SongMenu::SongMenu(const lib::spt::track &track, spt::Spotify &spotify, QWidget *parent)
	: SongMenu(track.id, track.artist, track.name, track.artist_id, track.album_id, 0,
	spotify, parent)
{
}

SongMenu::SongMenu(const std::string &trackId, std::string artist, std::string name,
	std::string artistId, std::string albumId, int index, spt::Spotify &spotify, QWidget *parent)
	: trackId(trackId),
	artist(std::move(artist)),
	trackName(std::move(name)),
	index(index),
	spotify(spotify),
	artistId(std::move(artistId)),
	albumId(std::move(albumId)),
	QMenu(parent)
{
	auto mainWindow = MainWindow::find(parent);
	if (mainWindow == nullptr)
	{
		lib::log::warn("Parent is not MainWindow, SongMenu won't work properly");
		return;
	}

	trackUri = lib::strings::starts_with(trackId, "spotify:track:")
		? QString::fromStdString(trackId).remove(0, QString("spotify:track:").length())
		: QString::fromStdString(trackId);
	auto trackFeatures = addAction(Icon::get("view-statistics"), "Audio features");
	QAction::connect(trackFeatures, &QAction::triggered, this, &SongMenu::openTrackFeatures);

//	auto lyrics = addAction(Icon::get("view-media-lyrics"), "Lyrics");
//	QAction::connect(lyrics, &QAction::triggered, this, &SongMenu::openLyrics);

	auto share = addMenu(Icon::get("document-share"), "Share");
	auto shareSongLink = share->addAction("Copy song link");
	QAction::connect(shareSongLink, &QAction::triggered, [this, mainWindow](bool checked)
	{
		QApplication::clipboard()
			->setText(QString("https://open.spotify.com/track/%1").arg(trackUri));
		mainWindow->setStatus("Link copied to clipboard");
	});

	auto shareSongOpen = share->addAction("Open in Spotify");
	QAction::connect(shareSongOpen, &QAction::triggered, [this, mainWindow](bool checked)
	{
		Utils::openUrl(QString("https://open.spotify.com/track/%1").arg(trackUri),
			LinkType::Web, mainWindow);
	});

	// Add/remove liked
	addSeparator();
	auto likedTracks = mainWindow->loadTracksFromCache("liked");
	isLiked = false;
	for (const auto &likedTrack : likedTracks)
		if (likedTrack.id == trackUri.toStdString())
		{
			isLiked = true;
			break;
		}

	auto toggleLiked = addAction(Icon::get(isLiked
			? "starred-symbolic" : "non-starred-symbolic"),
		isLiked ? "Dislike" : "Like");
	QAction::connect(toggleLiked, &QAction::triggered, this, &SongMenu::like);

	// Add to queue
	auto addQueue = addAction(Icon::get("media-playlist-append"), "Add to queue");
	QAction::connect(addQueue, &QAction::triggered, this, &SongMenu::addToQueue);

	// Add to playlist
	addSeparator();
	auto addPlaylist = addMenu(Icon::get("list-add"), "Add to playlist");

	auto playlistItem = mainWindow->getCurrentPlaylistItem();
	if (playlistItem != nullptr)
	{
		currentPlaylist = &mainWindow->getPlaylist(playlistItem->data(RoleIndex).toInt());
	}

	auto currentUserId = mainWindow->getCurrentUser().id;
	for (auto &playlist : mainWindow->getPlaylists())
	{
		if (!playlist.collaborative && playlist.owner_id != currentUserId)
			continue;

		// Create main action
		auto action = addPlaylist->addAction(QString::fromStdString(playlist.name));
		action->setData(QString::fromStdString(playlist.id));
	}
	QMenu::connect(addPlaylist, &QMenu::triggered, this, &SongMenu::addToPlaylist);

	// Remove from playlist
	auto remPlaylist = addAction(Icon::get("list-remove"), "Remove from playlist");
	remPlaylist->setVisible(mainWindow->getCurrentPlaylistItem() != nullptr);
	QAction::connect(remPlaylist, &QAction::triggered, this, &SongMenu::remFromPlaylist);

	addSeparator();
	auto goArtist = addAction(Icon::get("view-media-artist"), "View artist");
	goArtist->setVisible(this->artistId.length() > 1);
	QAction::connect(goArtist, &QAction::triggered, this, &SongMenu::viewArtist);

	auto goAlbum = addAction(Icon::get("view-media-album-cover"), "Open album");
	goAlbum->setVisible(this->albumId.length() > 1);
	QAction::connect(goAlbum, &QAction::triggered, this, &SongMenu::openAlbum);
}

void SongMenu::like(bool)
{
	auto callback = [this](const std::string &status)
	{
		if (!status.empty())
		{
			auto mainWindow = MainWindow::find(this->parentWidget());
			mainWindow->status(lib::fmt::format("Failed to {}: {}",
				isLiked ? "dislike" : "like", status), true);
		}
	};

	if (isLiked)
		spotify.remove_saved_track(trackId, callback);
	else
		spotify.add_saved_track(trackId, callback);
}

void SongMenu::addToQueue(bool)
{
	auto uri = lib::spt::api::to_uri("track", trackId);
	spotify.add_to_queue(uri, [this](const std::string &status)
	{
		MainWindow::find(this->parentWidget())->status(status, true);
	});
}

void SongMenu::addToPlaylist(QAction *action)
{
	// Check if it's already in the playlist
	auto playlistId = action->data().toString().toStdString();

	spotify.playlist(playlistId, [this, playlistId](const lib::spt::playlist &playlist)
	{
		this->spotify.playlist_tracks(playlist,
			[this, playlistId](const std::vector<lib::spt::track> &tracks)
			{
				auto mainWindow = MainWindow::find(this->parentWidget());
				for (auto &item : tracks)
				{
					if (lib::strings::ends_with(trackId, item.id))
					{
						auto result = QMessageBox::information(mainWindow, "Duplicate",
							"Track is already in the playlist, do you want to add it anyway?",
							QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

						if (result == QMessageBox::No)
							return;
						break;
					}
				}

				// Actually add
				auto plTrack = lib::spt::api::to_uri("track", trackId);
				spotify.add_to_playlist(playlistId, plTrack, [mainWindow](const std::string &result)
				{
					if (result.empty())
						return;
					mainWindow->status(lib::fmt::format("Failed to add track to playlist: {}",
						result), true);
				});
			});
	});
}

void SongMenu::remFromPlaylist(bool)
{
	spotify.remove_from_playlist(currentPlaylist->id, trackId, index,
		[this](const std::string &status)
		{
			// Remove from Spotify
			auto mainWindow = MainWindow::find(this->parentWidget());

			if (!status.empty())
			{
				mainWindow->status(lib::fmt::format(
					"Failed to remove track from playlist: {}", status), true);
				return;
			}

			// Remove from interface
			QTreeWidgetItem *item = nullptr;
			int i;
			for (i = 0; i < mainWindow->getSongsTree()->topLevelItemCount(); i++)
			{
				item = mainWindow->getSongsTree()->topLevelItem(i);
				if (item->data(0, RoleTrackId).toString().toStdString() == trackId)
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
			mainWindow->status(lib::fmt::format("Removed {} - {} from \"{}\"",
				trackName, artist, currentPlaylist->name));
		});
}

void SongMenu::openTrackFeatures(bool)
{
	MainWindow::find(parentWidget())->openAudioFeaturesWidget(trackId, artist, trackName);
}

void SongMenu::openLyrics(bool)
{
	MainWindow::find(parentWidget())->openLyrics(artist, trackName);
}

void SongMenu::viewArtist(bool)
{
	MainWindow::find(parentWidget())->openArtist(artistId);
}

void SongMenu::openAlbum(bool)
{
	auto mainWindow = MainWindow::find(parentWidget());
	mainWindow->loadAlbum(albumId, lib::spt::api::to_uri("track", trackId));
}
