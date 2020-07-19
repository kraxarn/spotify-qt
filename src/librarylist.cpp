#include "librarylist.hpp"

LibraryList::LibraryList(spt::Spotify &spotify, QWidget *parent)
	: spotify(spotify), parent(parent), QTreeWidget(parent)
{
	addTopLevelItems(
		{
			Utils::treeItem(
				this,
				"Recently Played",
				"Most recently played tracks from any device",
				QStringList()),
			Utils::treeItem(
				this,
				"Liked",
				"Liked and saved tracks",
				QStringList()),
			Utils::treeItem(
				this,
				"Tracks", ""
						  "Most played tracks for the past 6 months",
				QStringList()),
			Utils::treeItem(
				this,
				"New Releases",
				"New albums from artists you listen to",
				QStringList()),
			Utils::treeItem(
				this,
				"Albums",
				"Liked and saved albums"),
			Utils::treeItem(
				this,
				"Artists",
				"Most played artists for the past 6 months")
		}
	);

	header()->hide();
	setCurrentItem(nullptr);

	QTreeWidget::connect(
		this, &QTreeWidget::itemClicked, this, &LibraryList::clicked);
	QTreeWidget::connect(
		this, &QTreeWidget::itemDoubleClicked, this, &LibraryList::doubleClicked);
	QTreeWidget::connect(
		this, &QTreeWidget::itemExpanded, this, &LibraryList::expanded);
}

void LibraryList::clicked(QTreeWidgetItem *item, int)
{
	auto mainWindow = dynamic_cast<MainWindow *>(parent);
	if (mainWindow == nullptr)
		return;

	if (item != nullptr)
	{
		mainWindow->getPlaylistsList()->setCurrentRow(-1);
		if (item->parent() != nullptr)
		{
			auto data = item->data(0, 0x100).toString();
			switch (item->data(0, 0x101).toInt())
			{
				case Utils::RoleArtistId: mainWindow->openArtist(data);
					break;

				case Utils::RoleAlbumId: mainWindow->loadAlbum(data, false);
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

			QVector<spt::Track> tracks;
			if (item->text(0) == "Recently Played")
				tracks = spotify.recentlyPlayed();
			else if (item->text(0) == "Liked")
				tracks = spotify.savedTracks();
			else if (item->text(0) == "Tracks")
				tracks = spotify.topTracks();
			else if (item->text(0) == "New Releases")
			{
				auto all = mainWindow->allArtists();
				auto releases = spotify.newReleases();
				for (auto &album : releases)
					if (all.contains(album.artist))
						for (auto &track : spotify.albumTracks(album.id))
						{
							track.addedAt = album.releaseDate;
							tracks << track;
						}
			}

			mainWindow->saveTracksToCache(id, tracks);
			mainWindow->loadSongs(tracks);
			mainWindow->getSongsTree()->setEnabled(true);
		}
	}
}

void LibraryList::doubleClicked(QTreeWidgetItem *item, int)
{
	auto mainWindow = dynamic_cast<MainWindow*>(parent);
	if (mainWindow == nullptr)
		return;

	// Fetch all tracks in list
	auto tracks =
		item->text(0) == "Recently Played"
		? spotify.recentlyPlayed()
		: item->text(0) == "Liked"
		  ? spotify.savedTracks()
		  : item->text(0) == "Tracks"
			? spotify.topTracks()
			: QVector<spt::Track>();

	// If none were found, don't do anything
	if (tracks.isEmpty())
		return;

	// Get id of all tracks
	QStringList trackIds;
	tracks.reserve(tracks.length());
	for (auto &track : tracks)
		trackIds.append(QString("spotify:track:%1").arg(track.id));

	// Play in context of all tracks
	auto status = spotify.playTracks(trackIds.first(), trackIds);
	if (!status.isEmpty())
		mainWindow->setStatus(QString("Failed to start playback: %1").arg(status), true);
}

void LibraryList::expanded(QTreeWidgetItem *item)
{
	QVector<QVariantList> results;
	item->takeChildren();

	if (item->text(0) == "Artists")
		for (auto &artist : spotify.topArtists())
			results.append({artist.name, artist.id, Utils::RoleArtistId});
	else if (item->text(0) == "Albums")
		for (auto &album : spotify.savedAlbums())
			results.append({album.name, album.id, Utils::RoleAlbumId});

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
