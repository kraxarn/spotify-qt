#include "menu/track.hpp"
#include "mainwindow.hpp"
#include "dialog/createplaylist.hpp"

Menu::Track::Track(const lib::spt::track &track, lib::spt::api &spotify,
	const lib::cache &cache, QWidget *parent)
	: Menu::Track(track, spotify, cache, nullptr, parent)
{
}

Menu::Track::Track(const lib::spt::track &track, lib::spt::api &spotify,
	const lib::cache &cache, const lib::spt::artist *fromArtist, QWidget *parent)
	: Menu::Track(track, spotify, cache, fromArtist, -1, parent)
{
}

Menu::Track::Track(const lib::spt::track &track, lib::spt::api &spotify,
	const lib::cache &cache, int index, QWidget *parent)
	: Menu::Track(track, spotify, cache, nullptr, index, parent)
{
}

Menu::Track::Track(const lib::spt::track &track, lib::spt::api &spotify,
	const lib::cache &cache, const lib::spt::artist *fromArtist,
	int index, QWidget *parent)
	: QMenu(parent),
	spotify(spotify),
	track(track),
	index(index)
{
	auto *mainWindow = MainWindow::find(parent);
	if (mainWindow == nullptr)
	{
		return;
	}

	trackUri = lib::spt::api::to_id(track.id);
	auto *trackFeatures = addAction(Icon::get("view-statistics"), "Audio features");
	QAction::connect(trackFeatures, &QAction::triggered,
		this, &Menu::Track::onAudioFeatures);

	if (lib::developer_mode::enabled)
	{
		auto *lyrics = addAction(Icon::get("view-media-lyrics"), "Lyrics");
		QAction::connect(lyrics, &QAction::triggered,
			this, &Menu::Track::onLyrics);
	}

	auto *share = addMenu(Icon::get("document-share"), "Share");
	auto *shareSongLink = share->addAction("Copy song link");
	QAction::connect(shareSongLink, &QAction::triggered,
		this, &Menu::Track::onCopySongLink);

	auto *shareSongOpen = share->addAction("Open in Spotify");
	QAction::connect(shareSongOpen, &QAction::triggered,
		this, &Menu::Track::onOpenInSpotify);

	// Add/remove liked
	addSeparator();
	toggleLiked = addAction("Like");
	toggleLiked->setEnabled(false);
	setLiked(false);
	QAction::connect(toggleLiked, &QAction::triggered,
		this, &Menu::Track::onLike);

	spotify.is_saved_track({trackUri}, [this](const std::vector<bool> &likes)
	{
		auto liked = !likes.empty() && likes.front();
		this->setLiked(liked);
		this->toggleLiked->setEnabled(true);
	});

	// Add to queue
	auto *addQueue = addAction(Icon::get("media-playlist-append"), "Add to queue");
	QAction::connect(addQueue, &QAction::triggered,
		this, &Menu::Track::onAddToQueue);

	// Add to playlist
	addSeparator();

	auto *addPlaylist = addMenu(Icon::get("list-add"), "Add to playlist");

	auto *newPlaylist = addPlaylist->addAction("New playlist");
	newPlaylist->setData({});
	addPlaylist->addSeparator();

	auto *playlistItem = mainWindow->getCurrentPlaylistItem();
	if (playlistItem != nullptr)
	{
		const auto playlistIndex = playlistItem->data(static_cast<int>(DataRole::Index)).toInt();
		currentPlaylist = mainWindow->getPlaylist(playlistIndex);
	}

	auto currentUserId = mainWindow->getCurrentUser().id;
	for (auto &playlist: cache.get_playlists())
	{
		if (!playlist.collaborative && playlist.owner_id != currentUserId)
		{
			continue;
		}

		// Create main action
		auto *action = addPlaylist->addAction(QString::fromStdString(playlist.name));
		action->setData(QString::fromStdString(playlist.id));
	}

	QMenu::connect(addPlaylist, &QMenu::triggered,
		this, &Menu::Track::onAddToPlaylist);

	// Remove from playlist
	auto *remPlaylist = addAction(Icon::get("list-remove"), "Remove from playlist");
	remPlaylist->setVisible(index >= 0 && mainWindow->getCurrentPlaylistItem() != nullptr);

	QAction::connect(remPlaylist, &QAction::triggered,
		this, &Menu::Track::onRemoveFromPlaylist);

	addSeparator();
	if (track.artists.size() == 1 && fromArtist != nullptr
		&& fromArtist->id == track.artists.front().id)
	{
		// Do nothing, we're already in the only artist
	}
	else if (track.artists.size() > 1)
	{
		auto *artistsMenu = addMenu(Icon::get("view-media-artist"), "View artist");
		for (const auto &artist: track.artists)
		{
			if (fromArtist != nullptr && fromArtist->id == artist.id)
			{
				continue;
			}

			auto *goArtist = artistsMenu->addAction(QString::fromStdString(artist.name));
			QAction::connect(goArtist, &QAction::triggered, [this, artist](bool /*checked*/)
			{
				this->viewArtist(artist);
			});
		}
	}
	else if (!track.artists.empty())
	{
		const auto &artist = track.artists.front();
		auto *goArtist = addAction(Icon::get("view-media-artist"), "View artist");
		goArtist->setVisible(!artist.id.empty());
		QAction::connect(goArtist, &QAction::triggered, [this, artist](bool /*checked*/)
		{
			this->viewArtist(artist);
		});
	}

	auto *goAlbum = addAction(Icon::get("view-media-album-cover"), "Open album");
	goAlbum->setVisible(track.album.id.length() > 1);
	QAction::connect(goAlbum, &QAction::triggered,
		this, &Menu::Track::onOpenAlbum);
}

void Menu::Track::onLike(bool /*checked*/)
{
	auto callback = [this](const std::string &status)
	{
		if (status.empty())
		{
			return;
		}

		StatusMessage::error(QString("Failed to %1: %2")
			.arg(isLiked ? "dislike" : "onLike")
			.arg(QString::fromStdString(status)));
	};

	if (isLiked)
	{
		spotify.remove_saved_track(track.id, callback);
	}
	else
	{
		spotify.add_saved_track(track.id, callback);
	}
}

void Menu::Track::onAddToQueue(bool /*checked*/)
{
	auto uri = lib::spt::api::to_uri("track", track.id);
	spotify.add_to_queue(uri, [](const std::string &status)
	{
		StatusMessage::error(QString::fromStdString(status));
	});
}

void Menu::Track::onAddToPlaylist(QAction *action)
{
	const auto &data = action->data();
	if (!data.isValid())
	{
		addToNewPlaylist();
	}
	else
	{
		addToPlaylist(data.toString().toStdString());
	}
}

void Menu::Track::onRemoveFromPlaylist(bool /*checked*/)
{
	spotify.remove_from_playlist(currentPlaylist.id,
		lib::spt::api::to_uri("track", track.id), index,
		[this](const std::string &status)
		{
			// Remove from Spotify
			if (!status.empty())
			{
				StatusMessage::error(QString("Failed to remove track from playlist: %1")
					.arg(QString::fromStdString(status)));
				return;
			}

			// Remove from interface
			auto *mainWindow = MainWindow::find(this->parentWidget());
			QTreeWidgetItem *item = nullptr;
			int i;
			for (i = 0; i < mainWindow->getSongsTree()->topLevelItemCount(); i++)
			{
				item = mainWindow->getSongsTree()->topLevelItem(i);
				if (item->data(0, static_cast<int>(DataRole::Track))
					.value<lib::spt::track>().id == track.id)
				{
					break;
				}
				item = nullptr;
			}

			if (item == nullptr)
			{
				StatusMessage::warn(QStringLiteral("Failed to remove track from list"));
				return;
			}

			// It doesn't necessarily match item index depending on sorting order
			mainWindow->getSongsTree()->takeTopLevelItem(i);

			// Refresh the playlist automatically to prevent issues with songs being skipped
			mainWindow->getSongsTree()->refreshPlaylist(currentPlaylist);

			StatusMessage::info(QString("Removed %1 from \"%2\"")
				.arg(QString::fromStdString(track.title()))
				.arg(QString::fromStdString(currentPlaylist.name)));
		});
}

void Menu::Track::onAudioFeatures(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->openAudioFeaturesWidget(track);
}

void Menu::Track::onLyrics(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->openLyrics(track);
}

void Menu::Track::viewArtist(const lib::spt::entity &artist)
{
	MainWindow::find(parentWidget())->openArtist(artist.id);
}

void Menu::Track::onOpenAlbum(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->loadAlbum(track.album.id, lib::spt::api::to_uri("track", track.id));
}

void Menu::Track::setLiked(bool liked)
{
	isLiked = liked;

	toggleLiked->setIcon(Icon::get(liked
		? "starred-symbolic" : "non-starred-symbolic"));
	toggleLiked->setText(liked
		? "Unlike" : "Like");
}

auto Menu::Track::getTrackUrl() const -> QString
{
	auto str = lib::fmt::format("https://open.spotify.com/track/{}", trackUri);
	return QString::fromStdString(str);
}

void Menu::Track::addToNewPlaylist()
{
	auto *createPlaylist = new Dialog::CreatePlaylist(track.id, spotify, window());
	createPlaylist->open();
}

void Menu::Track::addToPlaylist(const std::string &playlistId) const
{
	// Check if it's already in the playlist
	spotify.playlist(playlistId, [this, playlistId](const lib::spt::playlist &playlist)
	{
		const auto playlistName = QString::fromStdString(playlist.name);

		this->spotify.playlist_tracks(playlist,
			[this, playlistId, playlistName](const std::vector<lib::spt::track> &tracks)
			{
				auto *mainWindow = MainWindow::find(this->parentWidget());
				for (const auto &item: tracks)
				{
					if (lib::strings::ends_with(track.id, item.id))
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
				auto plTrack = lib::spt::api::to_uri("track", track.id);
				spotify.add_to_playlist(playlistId, plTrack,
					[playlistName](const std::string &result)
					{
						if (!result.empty())
						{
							StatusMessage::error(QString("Failed to add track to playlist: %1")
								.arg(QString::fromStdString(result)));
							return;
						}

						StatusMessage::info(QString("Added to %1").arg(playlistName));
					});
			});
	});
}

void Menu::Track::onCopySongLink(bool /*checked*/)
{
	QApplication::clipboard()->setText(getTrackUrl());
	StatusMessage::info(QStringLiteral("Link copied to clipboard"));
}

void Menu::Track::onOpenInSpotify(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	Url::open(getTrackUrl(), LinkType::Web, mainWindow);
}
