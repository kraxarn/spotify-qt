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

	QTreeWidget::connect(this, &QTreeWidget::itemClicked, this, &LibraryList::clicked);
	QTreeWidget::connect(this, &QTreeWidget::itemDoubleClicked, this, &LibraryList::doubleClicked);
	QTreeWidget::connect(this, &QTreeWidget::itemExpanded, this, &LibraryList::expanded);
}

void LibraryList::clicked(QTreeWidgetItem *item, int)
{
	auto mainWindow = MainWindow::find(parentWidget());
	if (mainWindow == nullptr || item == nullptr)
		return;

	mainWindow->setCurrentPlaylistItem(-1);
	if (item->parent() != nullptr)
	{
		auto data = item->data(0, 0x100).toString();
		switch (item->data(0, 0x101).toInt())
		{
			case RoleArtistId:
				mainWindow->openArtist(data);
				break;

			case RoleAlbumId:
				mainWindow->loadAlbum(data);
				break;
		}
	}
	else
	{
		auto id = item->text(0).toLower().replace(' ', '_');
		auto cacheTracks = mainWindow->loadTracksFromCache(id);
		if (cacheTracks.isEmpty())
			mainWindow->getSongsTree()->setEnabled(false);
		else
			mainWindow->loadSongs(cacheTracks);

		auto callback = [mainWindow, id](const std::vector<spt::Track> &tracks)
		{
			if (!tracks.empty())
			{
				auto qTracks = QVector<spt::Track>(tracks.begin(), tracks.end());
				mainWindow->saveTracksToCache(id, qTracks);
				mainWindow->loadSongs(qTracks);
			}
			mainWindow->getSongsTree()->setEnabled(true);
		};

		if (item->text(0) == RECENTLY_PLAYED)
			spotify.recentlyPlayed(callback);
		else if (item->text(0) == SAVED_TRACKS)
			spotify.savedTracks(callback);
		else if (item->text(0) == TOP_TRACKS)
			spotify.topTracks(callback);
		else if (item->text(0) == NEW_RELEASES)
		{
			spotify.newReleases([this, mainWindow, callback]
				(const std::vector<spt::Album> &releases)
			{
				auto all = mainWindow->allArtists();
				std::vector<spt::Track> tracks;
				auto count = releases.size();

				for (auto &album : releases)
				{
					if (all.contains(album.artist))
					{
						this->spotify.albumTracks(album.id, [&tracks, album, &count, callback]
							(const std::vector<spt::Track> &results)
						{
							for (auto &result : results)
							{
								spt::Track track(result);
								track.addedAt = album.releaseDate;
								tracks.push_back(track);
							}

							if (--count <= 0)
							{
								callback(tracks);
							}
						});
					}
				}
			});
		}
	}
}

void LibraryList::doubleClicked(QTreeWidgetItem *item, int)
{
	auto mainWindow = MainWindow::find(parentWidget());
	if (mainWindow == nullptr)
		return;

	auto callback = [this, mainWindow](const std::vector<spt::Track> &tracks)
	{
		// If none were found, don't do anything
		if (tracks.empty())
			return;

		// Get id of all tracks
		QStringList trackIds;
		for (auto &track : tracks)
			trackIds.append(QString("spotify:track:%1").arg(track.id));

		// Play in context of all tracks
		this->spotify.playTracks(0, trackIds, [mainWindow](const QString &status)
		{
			if (!status.isEmpty())
				mainWindow->setStatus(QString("Failed to start playback: %1")
					.arg(status), true);
		});
	};

	// Fetch all tracks in list
	if (item->text(0) == RECENTLY_PLAYED)
		spotify.recentlyPlayed(callback);
	else if (item->text(0) == SAVED_TRACKS)
		spotify.savedTracks(callback);
	else if (item->text(0) == TOP_TRACKS)
		spotify.topTracks(callback);
}

void LibraryList::expanded(QTreeWidgetItem *item)
{
	auto callback = [item](const QVector<QVariantList> &results)
	{
		item->takeChildren();

		std::sort(results.begin(), results.end(),
			[](const QVariantList &x, const QVariantList &y)
			{
				return x.first().toString() < y.first().toString();
			}
		);

		// No results
		if (results.isEmpty())
		{
			auto child = new QTreeWidgetItem(item, {"No results"});
			child->setDisabled(true);
			child->setToolTip(0, "If they should be here, try logging out and back in");
			item->addChild(child);
			return;
		}

		// Add all to the list
		for (auto &result : results)
		{
			auto child = new QTreeWidgetItem(item, {result[0].toString()});
			child->setData(0, 0x100, result[1]);
			child->setData(0, 0x101, result[2]);
			item->addChild(child);
		}
	};

	QVector<QVariantList> results;

	if (item->text(0) == TOP_ARTISTS)
	{
		spotify.topArtists([callback](const std::vector<spt::Artist> &artists)
		{
			QVector<QVariantList> results;
			for (auto &artist : artists)
			{
				results.append({
					artist.name, artist.id, RoleArtistId
				});
			}
			callback(results);
		});
	}
	else if (item->text(0) == SAVED_ALBUMS)
	{
		spotify.savedAlbums([callback](const std::vector<spt::Album> &albums)
		{
			QVector<QVariantList> results;
			for (auto &album : albums)
			{
				results.append({
					album.name, album.id, RoleAlbumId
				});
			}
			callback(results);
		});
	}
	else if (item->text(0) == FOLLOWED_ARTISTS)
	{
		spotify.followedArtists([callback](const std::vector<spt::Artist> &artists)
		{
			QVector<QVariantList> results;
			for (auto &artist : artists)
			{
				results.append({
					artist.name, artist.id, RoleArtistId
				});
			}
			callback(results);
		});
	}
}
