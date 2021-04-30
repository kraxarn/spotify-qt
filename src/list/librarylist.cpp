#include "librarylist.hpp"

#include "../mainwindow.hpp"

#define FOLLOWED_ARTISTS "Followed Artists"
#define NEW_RELEASES "New Releases"
#define RECENTLY_PLAYED "History"
#define SAVED_ALBUMS "Liked Albums"
#define SAVED_TRACKS "Liked Tracks"
#define TOP_ARTISTS "Top Artists"
#define TOP_TRACKS "Top Tracks"

LibraryList::LibraryList(spt::Spotify &spotify, QWidget *parent)
	: spotify(spotify),
	QTreeWidget(parent)
{
	addTopLevelItems({
		Utils::treeItemWithChildren(this, RECENTLY_PLAYED,
			"Most recently played tracks from any device",
			QStringList()),
		Utils::treeItemWithChildren(this, SAVED_TRACKS,
			"Liked and saved tracks",
			QStringList()),
		Utils::treeItemWithChildren(this, TOP_TRACKS,
			"Most played tracks for the past 6 months",
			QStringList()),
		Utils::treeItemWithChildren(this, NEW_RELEASES,
			"New albums from artists you listen to",
			QStringList()),
		Utils::treeItemWithChildren(this, SAVED_ALBUMS,
			"Liked and saved albums"),
		Utils::treeItemWithChildren(this, TOP_ARTISTS,
			"Most played artists for the past 6 months"),
		Utils::treeItemWithChildren(this, FOLLOWED_ARTISTS,
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
	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow == nullptr || item == nullptr)
	{
		return;
	}

	mainWindow->setCurrentPlaylistItem(-1);
	if (item->parent() != nullptr)
	{
		auto *artistItem = dynamic_cast<LibraryArtistItem *>(item);
		if (artistItem != nullptr)
		{
			mainWindow->openArtist(artistItem->getData().id);
			return;
		}

		auto *albumItem = dynamic_cast<LibraryAlbumItem *>(item);
		if (albumItem != nullptr)
		{
			mainWindow->loadAlbum(albumItem->getData().id);
			return;
		}
	}
	else
	{
		auto id = item->text(0).toLower().replace(' ', '_').toStdString();
		auto cacheTracks = mainWindow->loadTracksFromCache(id);
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

		if (item->text(0) == RECENTLY_PLAYED)
		{
			spotify.recently_played(callback);
		}
		else if (item->text(0) == SAVED_TRACKS)
		{
			spotify.saved_tracks(callback);
		}
		else if (item->text(0) == TOP_TRACKS)
		{
			spotify.top_tracks(callback);
		}
		else if (item->text(0) == NEW_RELEASES)
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
	}
	mainWindow->getSongsTree()->setEnabled(true);
}

void LibraryList::doubleClicked(QTreeWidgetItem *item, int /*column*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow == nullptr)
	{
		return;
	}

	auto callback = [this, mainWindow](const std::vector<lib::spt::track> &tracks)
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
		this->spotify.play_tracks(0, trackIds,
			[mainWindow](const std::string &status)
			{
				if (!status.empty())
				{
					mainWindow->status(lib::fmt::format("Failed to start playback: {}",
						status), true);
				}
			});
	};

	// Fetch all tracks in list
	if (item->text(0) == RECENTLY_PLAYED)
	{
		spotify.recently_played(callback);
	}
	else if (item->text(0) == SAVED_TRACKS)
	{
		spotify.saved_tracks(callback);
	}
	else if (item->text(0) == TOP_TRACKS)
	{
		spotify.top_tracks(callback);
	}
}

template<typename T>
void itemsLoaded(const std::vector<T> &entities, QTreeWidgetItem *parent)
{
	std::vector<T> items = entities;
	std::sort(items.begin(), items.end(),
		[](const lib::spt::entity &x, const lib::spt::entity &y) -> bool
		{
			return x.name < y.name;
		}
	);

	// No results
	if (items.empty())
	{
		auto *child = new QTreeWidgetItem(parent, {
			"No results"
		});
		child->setDisabled(true);
		child->setToolTip(0, "If they should be here, try logging out and back in");
		parent->addChild(child);
		return;
	}

	// Add all to the list
	for (const auto &item : items)
	{
		parent->addChild(std::is_same<T, lib::spt::artist>::value
			? new LibraryArtistItem(item, parent)
			: std::is_same<T, lib::spt::album>::value
				? new LibraryAlbumItem(item, parent)
				: nullptr);
	}
}

void LibraryList::expanded(QTreeWidgetItem *item)
{
	item->takeChildren();

	if (item->text(0) == TOP_ARTISTS)
	{
		spotify.top_artists([item](const std::vector<lib::spt::artist> &artists)
		{
			itemsLoaded(artists, item);
		});
	}
	else if (item->text(0) == SAVED_ALBUMS)
	{
		spotify.saved_albums([item](const std::vector<lib::spt::saved_album> &albums)
		{
			itemsLoaded(albums, item);
		});
	}
	else if (item->text(0) == FOLLOWED_ARTISTS)
	{
		spotify.followed_artists([item](const std::vector<lib::spt::artist> &artists)
		{
			itemsLoaded(artists, item);
		});
	}
}
