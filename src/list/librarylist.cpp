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

void LibraryList::clicked(QTreeWidgetItem *item, int)
{
	auto mainWindow = MainWindow::find(parentWidget());
	if (mainWindow == nullptr || item == nullptr)
		return;

	mainWindow->setCurrentPlaylistItem(-1);
	if (item->parent() != nullptr)
	{
		auto data = item->data(0, 0x100).toString().toStdString();
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
		auto id = item->text(0).toLower().replace(' ', '_').toStdString();
		auto cacheTracks = mainWindow->loadTracksFromCache(id);
		auto songs = mainWindow->getSongsTree();

		if (cacheTracks.empty())
			songs->setEnabled(false);
		else
			songs->load(cacheTracks);

		auto callback = [this, id](const std::vector<lib::spt::track> &tracks)
		{
			this->tracksLoaded(id, tracks);
		};

		if (item->text(0) == RECENTLY_PLAYED)
			spotify.recently_played(callback);
		else if (item->text(0) == SAVED_TRACKS)
			callback(spotify.savedTracks());
		else if (item->text(0) == TOP_TRACKS)
			callback(spotify.topTracks());
		else if (item->text(0) == NEW_RELEASES)
		{
			auto all = mainWindow->allArtists();
			auto releases = spotify.newReleases();
			std::vector<lib::spt::track> tracks;

			for (auto &album : releases)
			{
				if (all.find(album.artist) != all.end())
				{
					for (auto &track : spotify.albumTracks(album.id))
					{
						track.added_at = album.release_date;
						tracks.push_back(track);
					}
				}
			}
			callback(tracks);
		}
	}
}

void LibraryList::tracksLoaded(const std::string &id, const std::vector<lib::spt::track> &tracks)
{
	auto mainWindow = MainWindow::find(parentWidget());

	if (!tracks.empty())
	{
		mainWindow->saveTracksToCache(id, tracks);
		mainWindow->getSongsTree()->load(tracks);
	}
	mainWindow->getSongsTree()->setEnabled(true);
}

void LibraryList::doubleClicked(QTreeWidgetItem *item, int)
{
	auto mainWindow = MainWindow::find(parentWidget());
	if (mainWindow == nullptr)
		return;

	auto callback = [this, mainWindow](const std::vector<lib::spt::track> &tracks)
	{
		// If none were found, don't do anything
		if (tracks.empty())
			return;

		// Get id of all tracks
		std::vector<std::string> trackIds;
		for (auto &track : tracks)
			trackIds.push_back(lib::spt::spotify_api::to_uri("track", track.id));

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
		spotify.recently_played(callback);
	else if (item->text(0) == SAVED_TRACKS)
		callback(spotify.savedTracks());
	else if (item->text(0) == TOP_TRACKS)
		callback(spotify.topTracks());
}

void LibraryList::expanded(QTreeWidgetItem *item)
{
	QVector<QVariantList> results;
	item->takeChildren();

	if (item->text(0) == TOP_ARTISTS)
		for (auto &artist : spotify.topArtists())
			results.append({artist.name, artist.id, RoleArtistId});
	else if (item->text(0) == SAVED_ALBUMS)
		for (auto &album : spotify.savedAlbums())
			results.append({QString::fromStdString(album.name),
				QString::fromStdString(album.id), RoleAlbumId});
	else if (item->text(0) == FOLLOWED_ARTISTS)
		for (auto &artist : spotify.followedArtists())
			results.append({artist.name, artist.id, RoleArtistId});

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
}
