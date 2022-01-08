#include "list/library.hpp"
#include "mainwindow.hpp"
#include "enum/libraryrole.hpp"

List::Library::Library(lib::spt::api &spotify, QWidget *parent)
	: QTreeWidget(parent),
	spotify(spotify)
{
	addTopLevelItems({
		Tree::itemWithNoChildren(this, recentlyPlayed,
			"Most recently played tracks from any device"),
		Tree::itemWithNoChildren(this, savedTracks,
			"Liked and saved tracks"),
		Tree::itemWithNoChildren(this, topTracks,
			"Most played tracks for the past 6 months"),
		Tree::itemWithNoChildren(this, newReleases,
			"New albums from artists you listen to"),
		Tree::itemWithEmptyChild(this, savedAlbums,
			"Liked and saved albums"),
		Tree::itemWithEmptyChild(this, topArtists,
			"Most played artists for the past 6 months"),
		Tree::itemWithEmptyChild(this, followedArtists,
			"Artists you're currently following")
	});

	header()->hide();
	setCurrentItem(nullptr);

	QTreeWidget::connect(this, &QTreeWidget::itemClicked,
		this, &List::Library::clicked);
	QTreeWidget::connect(this, &QTreeWidget::itemDoubleClicked,
		this, &List::Library::doubleClicked);
	QTreeWidget::connect(this, &QTreeWidget::itemExpanded,
		this, &List::Library::expanded);
}

void List::Library::clicked(QTreeWidgetItem *item, int /*column*/)
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
		const auto entityId = item->data(0, static_cast<int>(LibraryRole::EntityId))
			.toString().toStdString();

		const auto dataRole = item->data(0, static_cast<int>(LibraryRole::DataRole))
			.toInt();

		switch (static_cast<DataRole>(dataRole))
		{
			case DataRole::ArtistId:
				mainWindow->openArtist(entityId);
				break;

			case DataRole::AlbumId:
				mainWindow->loadAlbum(entityId);
				break;

			case DataRole::Track:
			case DataRole::Index:
			case DataRole::PlaylistId:
			case DataRole::AddedDate:
			case DataRole::Length:
			case DataRole::DefaultIndex:
			case DataRole::ShowId:
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

				for (const auto &album: releases)
				{
					if (all.find(album.artist) != all.end())
					{
						spotify.album_tracks(album,
							[album, callback](const std::vector<lib::spt::track> &results)
							{
								std::vector<lib::spt::track> tracks;
								tracks.reserve(results.size());
								for (const auto &result: results)
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

void List::Library::tracksLoaded(const std::string &id, const std::vector<lib::spt::track> &tracks)
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

void List::Library::doubleClicked(QTreeWidgetItem *item, int /*column*/)
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
		for (const auto &track: tracks)
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

void List::Library::expanded(QTreeWidgetItem *item)
{
	item->takeChildren();

	if (item->text(0) == topArtists)
	{
		spotify.top_artists([item](const std::vector<lib::spt::artist> &artists)
		{
			std::vector<ListItem::Library> results;
			results.reserve(artists.size());
			for (const auto &artist: artists)
			{
				results.emplace_back(artist, DataRole::ArtistId);
			}
			List::Library::itemsLoaded(results, item);
		});
	}
	else if (item->text(0) == savedAlbums)
	{
		spotify.saved_albums([item](const std::vector<lib::spt::saved_album> &savedAlbums)
		{
			std::vector<ListItem::Library> results;
			results.reserve(savedAlbums.size());
			for (const auto &savedAlbum: savedAlbums)
			{
				const auto &album = savedAlbum.album;
				const auto tooltip = lib::fmt::format("{}\nBy {}",
					album.name, album.artist);
				results.emplace_back(album, tooltip, DataRole::AlbumId);
			}
			List::Library::itemsLoaded(results, item);
		});
	}
	else if (item->text(0) == followedArtists)
	{
		spotify.followed_artists([item](const std::vector<lib::spt::artist> &artists)
		{
			std::vector<ListItem::Library> results;
			results.reserve(artists.size());
			for (const auto &artist: artists)
			{
				results.emplace_back(artist, DataRole::ArtistId);
			}
			List::Library::itemsLoaded(results, item);
		});
	}
}

void List::Library::itemsLoaded(std::vector<ListItem::Library> &items, QTreeWidgetItem *item)
{
	std::sort(items.begin(), items.end(),
		[](const ListItem::Library &item1, const ListItem::Library &item2) -> bool
		{
			return item1.getNameString() < item2.getNameString();
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
	for (auto &result: items)
	{
		auto *child = new QTreeWidgetItem(item, {
			result.getName(),
		});
		child->setToolTip(0, result.getTooltip());
		child->setData(0, static_cast<int>(LibraryRole::EntityId), result.getId());
		child->setData(0, static_cast<int>(LibraryRole::DataRole), result.getRole());
		item->addChild(child);
	}
}
