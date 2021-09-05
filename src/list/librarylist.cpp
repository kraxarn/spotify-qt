#include "librarylist.hpp"
#include "mainwindow.hpp"

LibraryList::LibraryList(spt::Spotify &spotify, QWidget *parent)
	: spotify(spotify),
	QTreeWidget(parent)
{
	addTopLevelItems({
		TreeUtils::itemWithNoChildren(this, recentlyPlayed,
			"Most recently played tracks from any device"),
		TreeUtils::itemWithNoChildren(this, savedTracks,
			"Liked and saved tracks"),
		TreeUtils::itemWithNoChildren(this, topTracks,
			"Most played tracks for the past 6 months"),
		TreeUtils::itemWithNoChildren(this, newReleases,
			"New albums from artists you listen to"),
		TreeUtils::itemWithEmptyChild(this, savedAlbums,
			"Liked and saved albums"),
		TreeUtils::itemWithEmptyChild(this, topArtists,
			"Most played artists for the past 6 months"),
		TreeUtils::itemWithEmptyChild(this, followedArtists,
			"Artists you're currently following")
	});

	header()->hide();
	setCurrentItem(nullptr);

	QTreeWidget::connect(this, &QTreeWidget::itemClicked,
		this, &LibraryList::clicked);
	QTreeWidget::connect(this, &QTreeWidget::itemDoubleClicked,
		this, &LibraryList::doubleClicked);
	QTreeWidget::connect(this, &QTreeWidget::itemExpanded,
		this, &LibraryList::expanded);
}

void LibraryList::clicked(QTreeWidgetItem *item, int /*column*/)
{
	if (item != nullptr
		&& item->parent() == nullptr
		&& item->childCount() > 0)
	{
		item->setExpanded(true);
		return;
	}

	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow == nullptr || item == nullptr)
	{
		return;
	}

	mainWindow->setCurrentPlaylistItem(-1);
	if (item->parent() != nullptr)
	{
		auto data = item->data(0, 0x100).toString().toStdString();
		switch (static_cast<DataRole>(item->data(0, 0x101).toInt()))
		{
			case DataRole::ArtistId:
				mainWindow->openArtist(data);
				break;

			case DataRole::AlbumId:
				mainWindow->loadAlbum(data);
				break;
		}
	}
	else
	{
		auto id = item->text(0).toLower().replace(' ', '_').toStdString();
		const auto &cacheTracks = mainWindow->loadTracksFromCache(id);
		auto *songs = mainWindow->getSongsTree();

		if (cacheTracks.empty())
		{
			songs->setEnabled(false);
		}
		else
		{
			songs->load(cacheTracks);
		}

		auto callback = [this, id](const std::vector<lib::spt::track> &tracks)
		{
			this->tracksLoaded(id, tracks);
		};

		if (item->text(0) == recentlyPlayed)
		{
			spotify.recently_played(callback);
		}
		else if (item->text(0) == savedTracks)
		{
			spotify.saved_tracks(callback);
		}
		else if (item->text(0) == topTracks)
		{
			spotify.top_tracks(callback);
		}
		else if (item->text(0) == newReleases)
		{
			spotify.new_releases([this, mainWindow, callback]
				(const std::vector<lib::spt::album> &releases)
			{
				auto all = mainWindow->allArtists();
				std::vector<lib::spt::track> tracks;

				for (const auto &album : releases)
				{
					if (all.find(album.artist) != all.end())
					{
						spotify.album_tracks(album,
							[album, callback](const std::vector<lib::spt::track> &results)
							{
								std::vector<lib::spt::track> tracks;
								tracks.reserve(results.size());
								for (const auto &result : results)
								{
									lib::spt::track track = result;
									track.added_at = album.release_date;
									tracks.push_back(track);
								}
								callback(tracks);
							});
						return;
					}
				}
				callback(tracks);
			});
		}
	}
}

void LibraryList::tracksLoaded(const std::string &id, const std::vector<lib::spt::track> &tracks)
{
	auto *mainWindow = MainWindow::find(parentWidget());

	if (!tracks.empty())
	{
		mainWindow->saveTracksToCache(id, tracks);
		mainWindow->getSongsTree()->load(tracks);
		mainWindow->setNoSptContext();
	}
	mainWindow->getSongsTree()->setEnabled(true);
}

void LibraryList::doubleClicked(QTreeWidgetItem *item, int /*column*/)
{
	auto callback = [this](const std::vector<lib::spt::track> &tracks)
	{
		// If none were found, don't do anything
		if (tracks.empty())
		{
			return;
		}

		// Get id of all tracks
		std::vector<std::string> trackIds;
		trackIds.reserve(tracks.size());
		for (const auto &track : tracks)
		{
			trackIds.push_back(lib::spt::api::to_uri("track", track.id));
		}

		// Play in context of all tracks
		this->spotify.play_tracks(0, trackIds, [](const std::string &status)
		{
			if (status.empty())
			{
				return;
			}

			StatusMessage::error(QString("Failed to start playback: %1")
				.arg(QString::fromStdString(status)));
		});
	};

	// Fetch all tracks in list
	if (item->text(0) == recentlyPlayed)
	{
		spotify.recently_played(callback);
	}
	else if (item->text(0) == savedTracks)
	{
		spotify.saved_tracks(callback);
	}
	else if (item->text(0) == topTracks)
	{
		spotify.top_tracks(callback);
	}
}

void LibraryList::expanded(QTreeWidgetItem *item)
{
	item->takeChildren();

	if (item->text(0) == topArtists)
	{
		spotify.top_artists([item](const std::vector<lib::spt::artist> &artists)
		{
			std::vector<ListItem::Library> results;
			results.reserve(artists.size());
			for (const auto &artist : artists)
			{
				results.emplace_back(artist.name, artist.id, DataRole::ArtistId);
			}
			LibraryList::itemsLoaded(results, item);
		});
	}
	else if (item->text(0) == savedAlbums)
	{
		spotify.saved_albums([item](const std::vector<lib::spt::saved_album> &albums)
		{
			std::vector<ListItem::Library> results;
			results.reserve(albums.size());
			for (const auto &album : albums)
			{
				results.emplace_back(album.album.name, album.album.id, DataRole::AlbumId);
			}
			LibraryList::itemsLoaded(results, item);
		});
	}
	else if (item->text(0) == followedArtists)
	{
		spotify.followed_artists([item](const std::vector<lib::spt::artist> &artists)
		{
			std::vector<ListItem::Library> results;
			results.reserve(artists.size());
			for (const auto &artist : artists)
			{
				results.emplace_back(artist.name, artist.id, DataRole::ArtistId);
			}
			LibraryList::itemsLoaded(results, item);
		});
	}
}

void LibraryList::itemsLoaded(std::vector<ListItem::Library> &items, QTreeWidgetItem *item)
{
	std::sort(items.begin(), items.end(),
		[](const ListItem::Library &x, const ListItem::Library &y) -> bool
		{
			return x.name < y.name;
		}
	);

	// No results
	if (items.empty())
	{
		auto *child = new QTreeWidgetItem(item, {
			"No results"
		});
		child->setDisabled(true);
		child->setToolTip(0, "If they should be here, try logging out and back in");
		item->addChild(child);
		return;
	}

	// Add all to the list
	for (auto &result : items)
	{
		auto *child = new QTreeWidgetItem(item, {
			QString::fromStdString(result.name)
		});
		child->setData(0, 0x100, QString::fromStdString(result.id));
		child->setData(0, 0x101, static_cast<int>(result.role));
		item->addChild(child);
	}
}
