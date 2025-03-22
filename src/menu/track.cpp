#include "menu/track.hpp"

#include "mainwindow.hpp"
#include "menu/addtoplaylist.hpp"

Menu::Track::Track(const lib::spt::track &track, lib::spt::api &spotify,
	const lib::cache &cache, QWidget *parent)
	: Menu::Track({PlaylistTrack(-1, track)}, spotify, cache, nullptr, parent)
{
}

Menu::Track::Track(const QList<PlaylistTrack> &tracks, lib::spt::api &spotify,
	const lib::cache &cache, QWidget *parent)
	: Menu::Track(tracks, spotify, cache, nullptr, parent)
{
}

Menu::Track::Track(const QList<PlaylistTrack> &tracks, lib::spt::api &spotify,
	const lib::cache &cache, const lib::spt::artist *fromArtist, QWidget *parent)
	: QMenu(parent),
	spotify(spotify),
	tracks(tracks)
{
	if (tracks.empty())
	{
		return;
	}

	auto *mainWindow = MainWindow::find(parent);
	if (mainWindow == nullptr)
	{
		return;
	}

	const auto isSingle = tracks.length() == 1;
	const auto &singleTrack = tracks.at(0).second;

	if (tracks.length() > 1)
	{
		auto duration = 0;
		for (const auto &track: tracks)
		{
			duration += track.second.duration;
		}

		const auto countText = QStringLiteral("%1 tracks, %2")
			.arg(tracks.length())
			.arg(QString::fromStdString(lib::format::time_pretty(duration)));

		auto *countAction = addAction(countText);
		countAction->setEnabled(false);
		addSeparator();
	}

	if (isSingle)
	{
		auto *lyrics = addAction(Icon::get("view-media-lyrics"), "Lyrics");
		QAction::connect(lyrics, &QAction::triggered,
			this, &Menu::Track::onLyrics);

		auto *share = addMenu(Icon::get("document-share"), "Share");

		auto *shareSongLink = share->addAction("Copy song link");
		QAction::connect(shareSongLink, &QAction::triggered,
			this, &Menu::Track::onCopySongLink);

		auto *shareSongName = share->addAction("Copy song name");
		QAction::connect(shareSongName, &QAction::triggered,
			this, &Menu::Track::onCopySongName);

		auto *shareSongOpen = share->addAction("Open in Spotify");
		QAction::connect(shareSongOpen, &QAction::triggered,
			this, &Menu::Track::onOpenInSpotify);
	}

	// Add/remove liked
	addSeparator();
	toggleLiked = addAction("Like");
	toggleLiked->setEnabled(false);
	toggleLiked->setVisible(isSingle);
	setLiked(false);
	QAction::connect(toggleLiked, &QAction::triggered,
		this, &Menu::Track::onLike);

	if (isSingle)
	{
		const auto trackId = lib::spt::uri_to_id(singleTrack.id);
		spotify.is_saved_track({trackId}, [this](const std::vector<bool> &likes)
		{
			auto liked = !likes.empty() && likes.front();
			this->setLiked(liked);
			this->toggleLiked->setEnabled(true);
		});
	}

	// Add to queue
	auto *addQueue = addAction(Icon::get("media-playlist-append"), "Add to queue");
	QAction::connect(addQueue, &QAction::triggered,
		this, &Menu::Track::onAddToQueue);

	// Add to playlist
	addSeparator();
	addMenu(new Menu::AddToPlaylist(getTrackIds(), spotify, cache, this));

	auto *playlistItem = mainWindow->getCurrentPlaylistItem();
	if (playlistItem != nullptr)
	{
		const auto playlistIndex = playlistItem->data(static_cast<int>(DataRole::Index)).toInt();
		currentPlaylist = mainWindow->getPlaylist(playlistIndex);
	}

	// Remove from playlist
	const auto &currentUserId = mainWindow->getCurrentUser().id;
	auto *removeFromPlaylist = getRemoveFromPlaylistAction(currentUserId);
	if (removeFromPlaylist != nullptr)
	{
		addAction(removeFromPlaylist);
	}

	// View artist
	auto *artist = getArtistObject(fromArtist);
	if (artist != nullptr)
	{
		addSeparator();
		auto *artistAction = qobject_cast<QAction *>(artist);
		if (artistAction != nullptr)
		{
			addAction(artistAction);
		}
		else
		{
			auto *artistMenu = qobject_cast<QMenu *>(artist);
			if (artistMenu != nullptr)
			{
				addMenu(artistMenu);
			}
		}
	}

	// Open album
	auto *albumAction = getAlbumAction();
	if (albumAction != nullptr)
	{
		addAction(albumAction);
	}
}

auto Menu::Track::getRemoveFromPlaylistAction(const std::string &currentUserId) -> QAction *
{
	if (!anyInPlaylist()
		|| !currentPlaylist.is_valid()
		|| currentPlaylist.owner_id != currentUserId)
	{
		return nullptr;
	}

	const auto icon = Icon::get(QStringLiteral("list-remove"));
	const auto text = QStringLiteral("Remove from playlist");

	auto *action = new QAction(icon, text, this);

	QAction::connect(action, &QAction::triggered,
		this, &Menu::Track::onRemoveFromPlaylist);

	return action;
}

auto Menu::Track::getArtistObject(const lib::spt::artist *fromArtist) -> QObject *
{
	if (!allSameArtists())
	{
		return nullptr;
	}

	const auto &artists = tracks.cbegin()->second.artists;
	if (artists.empty())
	{
		return nullptr;
	}

	// Single artist, and we're on that artist
	if (artists.size() == 1
		&& fromArtist != nullptr
		&& fromArtist->id == artists.front().id)
	{
		return nullptr;
	}

	const auto text = QStringLiteral("View artist");
	const auto icon = Icon::get("view-media-artist");

	// Single artist
	if (artists.size() == 1)
	{
		const auto &artist = artists.front();
		auto *action = new QAction(icon, text, this);
		action->setVisible(!artist.id.empty());

		QAction::connect(action, &QAction::triggered, [this, artist](bool /*checked*/)
		{
			this->viewArtist(artist);
		});

		return action;
	}

	// Multiple artists
	auto *menu = new QMenu(text, this);
	menu->setIcon(icon);

	for (const auto &artist: artists)
	{
		if (fromArtist != nullptr && fromArtist->id == artist.id)
		{
			continue;
		}

		auto *goArtist = menu->addAction(QString::fromStdString(artist.name));
		QAction::connect(goArtist, &QAction::triggered, [this, artist](bool /*checked*/)
		{
			this->viewArtist(artist);
		});
	}

	return menu;
}

auto Menu::Track::getAlbumAction() -> QAction *
{
	if (!allSameAlbum())
	{
		return nullptr;
	}

	const auto &album = tracks.front().second.album;
	if (!album.is_valid())
	{
		return nullptr;
	}

	const auto icon = Icon::get("view-media-album-cover");
	const auto text = QStringLiteral("Open album");

	auto *action = new QAction(icon, text, this);

	QAction::connect(action, &QAction::triggered,
		this, &Menu::Track::onOpenAlbum);

	return action;
}

auto Menu::Track::anyInPlaylist() const -> bool
{
	return std::any_of(tracks.cbegin(), tracks.cend(), [](const PlaylistTrack &track) -> bool
	{
		return track.first >= 0;
	});
}

auto Menu::Track::getTrackIds() const -> std::vector<std::string>
{
	std::vector<std::string> trackIds;
	trackIds.reserve(tracks.size());

	for (const auto &track: tracks)
	{
		trackIds.push_back(track.second.id);
	}

	return trackIds;
}

auto Menu::Track::getTracks() const -> std::vector<lib::spt::track>
{
	std::vector<lib::spt::track> sptTracks;
	sptTracks.reserve(tracks.size());

	for (const auto &track: tracks)
	{
		sptTracks.push_back(track.second);
	}

	return sptTracks;
}

void Menu::Track::onLike(bool /*checked*/)
{
	auto callback = [this](const std::string &status)
	{
		if (status.empty())
		{
			return;
		}

		StatusMessage::error(QStringLiteral("Failed to %1: %2")
			.arg(isLiked ? "unlike" : "like")
			.arg(QString::fromStdString(status)));
	};

	if (isLiked)
	{
		spotify.remove_saved_tracks(getTrackIds(), callback);
	}
	else
	{
		spotify.add_saved_tracks(getTrackIds(), callback);
	}
}

void Menu::Track::addToQueue(const QList<PlaylistTrack>::const_iterator &begin,
	const QList<PlaylistTrack>::const_iterator &end)
{
	if (begin == end)
	{
		StatusMessage::info(tracks.size() == 1
			? QStringLiteral("Added to queue")
			: QStringLiteral("%1 tracks added to queue").arg(tracks.size()));

		return;
	}

	const auto uri = lib::spt::id_to_uri("track", begin->second.id);
	spotify.add_to_queue(uri, [this, begin, end](const lib::result<void *> &result)
	{
		if (!result.success())
		{
			StatusMessage::error(QStringLiteral("Failed to add to queue: %1")
				.arg(QString::fromStdString(result.message())));
			return;
		}

		addToQueue(begin + 1, end);
	});
}

void Menu::Track::onAddToQueue(bool /*checked*/)
{
	// The API only supports adding a single track at once
	addToQueue(tracks.cbegin(), tracks.cend());
}

void Menu::Track::onRemoveFromPlaylist(bool /*checked*/)
{
	std::vector<std::pair<int, std::string>> uris;
	uris.reserve(tracks.size());

	std::unordered_set<std::string> trackIds;
	trackIds.reserve(tracks.size());

	for (const auto &track: tracks)
	{
		uris.emplace_back(track.first, lib::spt::id_to_uri("track", track.second.id));
		trackIds.insert(track.second.id);
	}

	spotify.remove_from_playlist(currentPlaylist.id, uris,
		[this, trackIds](const std::string &status)
		{
			// Remove from Spotify
			if (!status.empty())
			{
				StatusMessage::error(QStringLiteral("Failed to remove track from playlist: %1")
					.arg(QString::fromStdString(status)));
				return;
			}

			// Remove from interface
			auto *mainWindow = MainWindow::find(this->parentWidget());
			QList<int> toRemove;

			for (auto i = 0; i < mainWindow->getSongsTree()->topLevelItemCount(); i++)
			{
				auto *item = mainWindow->getSongsTree()->topLevelItem(i);

				const auto &trackId = item->data(0,
					static_cast<int>(DataRole::Track)).value<lib::spt::track>().id;

				if (trackIds.count(trackId) > 0)
				{
					toRemove.append(i);
				}
			}

			if (toRemove.empty())
			{
				lib::log::warn("Failed to remove track from list");
				return;
			}

			// It doesn't necessarily match item index depending on sorting order
			for (const auto index: toRemove)
			{
				mainWindow->getSongsTree()->takeTopLevelItem(index);
			}

			// Refresh the playlist automatically to prevent issues with songs being skipped
			mainWindow->getSongsTree()->refreshPlaylist(currentPlaylist);

			StatusMessage::info(QStringLiteral("Removed from %1")
				.arg(QString::fromStdString(currentPlaylist.name)));
		});
}

void Menu::Track::onLyrics(bool /*checked*/)
{
	if (tracks.empty())
	{
		return;
	}

	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->openLyrics(tracks.cbegin()->second);
}

void Menu::Track::viewArtist(const lib::spt::entity &artist)
{
	MainWindow::find(parentWidget())->openArtist(artist.id);
}

void Menu::Track::onOpenAlbum(bool /*checked*/)
{
	if (tracks.empty())
	{
		return;
	}

	auto *mainWindow = MainWindow::find(parentWidget());
	const auto &track = tracks.cbegin()->second;

	mainWindow->loadAlbum(track.album.id, lib::spt::id_to_uri("track", track.id));
}

void Menu::Track::setLiked(bool liked)
{
	isLiked = liked;

	toggleLiked->setIcon(Icon::get(liked
		? QStringLiteral("starred-symbolic")
		: QStringLiteral("non-starred-symbolic")));

	toggleLiked->setText(liked
		? QStringLiteral("Unlike")
		: QStringLiteral("Like"));
}

auto Menu::Track::getTrackUrl() const -> QString
{
	if (tracks.empty())
	{
		return {};
	}

	const auto trackId = tracks.cbegin()->second.id;
	auto str = lib::fmt::format("https://open.spotify.com/track/{}", trackId);
	return QString::fromStdString(str);
}

auto Menu::Track::getTrackDisplayName() const -> QString
{
	if (tracks.empty())
	{
		return {};
	}

	const auto track = tracks.cbegin()->second;
	const auto trackName = track.name;
	const auto artistNames = lib::spt::entity::combine_names(track.artists);
	auto str = lib::fmt::format("{} - {}", artistNames, trackName);
	return QString::fromStdString(str);
}

void Menu::Track::onCopySongLink(bool /*checked*/)
{
	QApplication::clipboard()->setText(getTrackUrl());
	StatusMessage::info(QStringLiteral("Link copied to clipboard"));
}

void Menu::Track::onCopySongName(bool /*checked*/)
{
	QApplication::clipboard()->setText(getTrackDisplayName());
	StatusMessage::info(QStringLiteral("Name copied to clipboard"));
}

void Menu::Track::onOpenInSpotify(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	Url::open(getTrackUrl(), LinkType::Web, mainWindow);
}

auto Menu::Track::allSameArtists() const -> bool
{
	if (tracks.empty()
		|| tracks.cbegin()->second.artists.empty())
	{
		return false;
	}

	const auto &first = tracks.cbegin()->second.artists;
	for (auto iter = tracks.cbegin() + 1; iter != tracks.cend(); iter++)
	{
		// Same number of artists
		const auto &current = iter->second.artists;
		if (current.size() != first.size())
		{
			return false;
		}

		// Compare artists
		// TODO: This assumes all artists are in the same order
		for (size_t i = 0; i < first.size(); i++)
		{
			if (current.at(i).id != first.at(i).id)
			{
				return false;
			}
		}
	}

	return true;
}

auto Menu::Track::allSameAlbum() const -> bool
{
	if (tracks.empty())
	{
		return false;
	}

	const auto &first = tracks.cbegin()->second.album;
	for (auto iter = tracks.cbegin() + 1; iter != tracks.cend(); iter++)
	{
		// Albums may not have an id set, so use name instead
		if (first.name != iter->second.album.name)
		{
			return false;
		}
	}

	return true;
}
