#include "songmenu.hpp"

#include "mainwindow.hpp"

SongMenu::SongMenu(const lib::spt::track &track, spt::Spotify &spotify, QWidget *parent)
	: SongMenu(track, spotify, false, parent)
{
}

SongMenu::SongMenu(const lib::spt::track &track, spt::Spotify &spotify,
	bool forceArtistSubmenu, QWidget *parent)
	: SongMenu(track.id, track.artists, track.name, track.album.id, 0,
	spotify, forceArtistSubmenu, parent)
{
}

SongMenu::SongMenu(const std::string &trackId, const std::vector<lib::spt::entity> &artists,
	std::string name, std::string albumId, int index, spt::Spotify &spotify,
	bool forceArtistSubmenu, QWidget *parent)
	: trackId(trackId),
	artists(artists),
	trackName(std::move(name)),
	index(index),
	spotify(spotify),
	albumId(std::move(albumId)),
	QMenu(parent)
{
	auto *mainWindow = MainWindow::find(parent);
	if (mainWindow == nullptr)
	{
		lib::log::warn("Parent is not MainWindow, SongMenu won't work properly");
		return;
	}

	trackUri = lib::spt::api::to_id(trackId);
	auto *trackFeatures = addAction(Icon::get("view-statistics"), "Audio features");
	QAction::connect(trackFeatures, &QAction::triggered, this, &SongMenu::openTrackFeatures);

//	auto lyrics = addAction(Icon::get("view-media-lyrics"), "Lyrics");
//	QAction::connect(lyrics, &QAction::triggered, this, &SongMenu::openLyrics);

	auto *share = addMenu(Icon::get("document-share"), "Share");
	auto *shareSongLink = share->addAction("Copy song link");
	QAction::connect(shareSongLink, &QAction::triggered, [this, mainWindow](bool /*checked*/)
	{
		QApplication::clipboard()->setText(this->getTrackUrl());
		mainWindow->setStatus("Link copied to clipboard");
	});

	auto *shareSongOpen = share->addAction("Open in Spotify");
	QAction::connect(shareSongOpen, &QAction::triggered, [this, mainWindow](bool /*checked*/)
	{
		Utils::openUrl(this->getTrackUrl(), LinkType::Web, mainWindow);
	});

	// Add/remove liked
	addSeparator();
	toggleLiked = addAction("Like");
	toggleLiked->setEnabled(false);
	setLiked(false);
	QAction::connect(toggleLiked, &QAction::triggered, this, &SongMenu::like);

	spotify.is_saved_track({trackUri}, [this](const std::vector<bool> &likes)
	{
		auto liked = !likes.empty() && likes.front();
		this->setLiked(liked);
		this->toggleLiked->setEnabled(true);
	});

	// Add to queue
	auto *addQueue = addAction(Icon::get("media-playlist-append"), "Add to queue");
	QAction::connect(addQueue, &QAction::triggered, this, &SongMenu::addToQueue);

	// Add to playlist
	addSeparator();
	auto *addPlaylist = addMenu(Icon::get("list-add"), "Add to playlist");

	auto *playlistItem = mainWindow->getCurrentPlaylistItem();
	if (playlistItem != nullptr)
	{
		currentPlaylist = &mainWindow->getPlaylist(playlistItem->data(RoleIndex).toInt());
	}

	auto currentUserId = mainWindow->getCurrentUser().id;
	for (auto &playlist : mainWindow->getPlaylists())
	{
		if (!playlist.collaborative && playlist.owner_id != currentUserId)
		{
			continue;
		}

		// Create main action
		auto *action = addPlaylist->addAction(QString::fromStdString(playlist.name));
		action->setData(QString::fromStdString(playlist.id));
	}
	QMenu::connect(addPlaylist, &QMenu::triggered, this, &SongMenu::addToPlaylist);

	// Remove from playlist
	auto *remPlaylist = addAction(Icon::get("list-remove"), "Remove from playlist");
	remPlaylist->setVisible(mainWindow->getCurrentPlaylistItem() != nullptr);
	QAction::connect(remPlaylist, &QAction::triggered, this, &SongMenu::remFromPlaylist);

	addSeparator();
	if (artists.size() > 1 || (forceArtistSubmenu && !artists.empty()))
	{
		auto *artistsMenu = addMenu(Icon::get("view-media-artist"), "View artist");
		for (const auto &artist : artists)
		{
			auto *goArtist = artistsMenu->addAction(QString::fromStdString(artist.name));
			QAction::connect(goArtist, &QAction::triggered, [this, artist](bool /*checked*/)
			{
				this->viewArtist(artist);
			});
		}
	}
	else if (!artists.empty())
	{
		const auto &artist = this->artists.front();
		auto *goArtist = addAction(Icon::get("view-media-artist"), "View artist");
		goArtist->setVisible(!artist.id.empty());
		QAction::connect(goArtist, &QAction::triggered, [this, artist](bool /*checked*/)
		{
			this->viewArtist(artist);
		});
	}

	auto *goAlbum = addAction(Icon::get("view-media-album-cover"), "Open album");
	goAlbum->setVisible(this->albumId.length() > 1);
	QAction::connect(goAlbum, &QAction::triggered, this, &SongMenu::openAlbum);
}

void SongMenu::like(bool /*checked*/)
{
	auto callback = [this](const std::string &status)
	{
		if (!status.empty())
		{
			auto *mainWindow = MainWindow::find(this->parentWidget());
			mainWindow->status(lib::fmt::format("Failed to {}: {}",
				isLiked ? "dislike" : "like", status), true);
		}
	};

	if (isLiked)
	{
		spotify.remove_saved_track(trackId, callback);
	}
	else
	{
		spotify.add_saved_track(trackId, callback);
	}
}

void SongMenu::addToQueue(bool /*checked*/)
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
				auto *mainWindow = MainWindow::find(this->parentWidget());
				for (const auto &item : tracks)
				{
					if (lib::strings::ends_with(trackId, item.id))
					{
						auto result = QMessageBox::information(mainWindow, "Duplicate",
							"Track is already in the playlist, do you want to add it anyway?",
							QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

						if (result == QMessageBox::No)
						{
							return;
						}
						break;
					}
				}

				// Actually add
				auto plTrack = lib::spt::api::to_uri("track", trackId);
				spotify.add_to_playlist(playlistId, plTrack, [mainWindow](const std::string &result)
				{
					if (result.empty())
					{
						return;
					}
					mainWindow->status(lib::fmt::format("Failed to add track to playlist: {}",
						result), true);
				});
			});
	});
}

void SongMenu::remFromPlaylist(bool /*checked*/)
{
	spotify.remove_from_playlist(currentPlaylist->id, trackId, index,
		[this](const std::string &status)
		{
			// Remove from Spotify
			auto *mainWindow = MainWindow::find(this->parentWidget());

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
				if (item->data(0, RoleTrack).value<lib::spt::track>().id == trackId)
				{
					break;
				}
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
				trackName,
				lib::spt::entity::combine_names(artists),
				currentPlaylist->name));
		});
}

void SongMenu::openTrackFeatures(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->openAudioFeaturesWidget(trackId,
		lib::spt::entity::combine_names(artists), trackName);
}

void SongMenu::openLyrics(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->openLyrics(lib::spt::entity::combine_names(artists), trackName);
}

void SongMenu::viewArtist(const lib::spt::entity &artist)
{
	MainWindow::find(parentWidget())->openArtist(artist.id);
}

void SongMenu::openAlbum(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->loadAlbum(albumId, lib::spt::api::to_uri("track", trackId));
}

void SongMenu::setLiked(bool liked)
{
	isLiked = liked;

	toggleLiked->setIcon(Icon::get(liked
		? "starred-symbolic" : "non-starred-symbolic"));
	toggleLiked->setText(liked
		? "Dislike" : "Like");
}

auto SongMenu::getTrackUrl() const -> QString
{
	auto str = lib::fmt::format("https://open.spotify.com/track/{}", trackUri);
	return QString::fromStdString(str);
}
