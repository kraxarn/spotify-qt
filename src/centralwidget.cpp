#include "centralwidget.hpp"

CentralWidget::CentralWidget(spt::Spotify &spotify, QWidget *parent)
	: spotify(spotify), QSplitter(parent)
{
	mainWindow = dynamic_cast<MainWindow*>(parent);
	if (mainWindow == nullptr)
		return;

	// Sidebar with playlists etc.
	auto sidebar = new QVBoxLayout();
	libraryList = new QTreeWidget(this);
	playlists = new QListWidget(this);
	// Library
	libraryList->addTopLevelItems(
		{
			Utils::treeItem(
				libraryList,
				"Recently Played",
				"Most recently played tracks from any device",
				QStringList()),
			Utils::treeItem(
				libraryList, "Liked",
				"Liked and saved tracks",
				QStringList()),
			Utils::treeItem(
				libraryList,
				"Tracks",
				"Most played tracks for the past 6 months",
				QStringList()),
			Utils::treeItem(
				libraryList,
				"New Releases",
				"New albums from artists you listen to",
				QStringList()),
			Utils::treeItem(
				libraryList,
				"Albums",
				"Liked and saved albums"),
			Utils::treeItem(
				libraryList,
				"Artists",
				"Most played artists for the past 6 months")
		}
	);
	libraryList->header()->hide();
	libraryList->setCurrentItem(nullptr);

	QTreeWidget::connect(libraryList, &QTreeWidget::itemClicked, [this](QTreeWidgetItem *item, int column)
	{
		if (item != nullptr)
		{
			playlists->setCurrentRow(-1);
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
					songs->setEnabled(false);
				else
					mainWindow->loadSongs(cacheTracks);

				auto spotify = mainWindow->getSpotify();
				QVector<spt::Track> tracks;
				if (item->text(0) == "Recently Played")
					tracks = spotify->recentlyPlayed();
				else if (item->text(0) == "Liked")
					tracks = spotify->savedTracks();
				else if (item->text(0) == "Tracks")
					tracks = spotify->topTracks();
				else if (item->text(0) == "New Releases")
				{
					auto all = mainWindow->allArtists();
					auto releases = spotify->newReleases();
					for (auto &album : releases)
						if (all.contains(album.artist))
							for (auto &track : spotify->albumTracks(album.id))
							{
								track.addedAt = album.releaseDate;
								tracks << track;
							}
				}

				mainWindow->saveTracksToCache(id, tracks);
				mainWindow->loadSongs(tracks);
				songs->setEnabled(true);
			}
		}
	});

	QTreeWidget::connect(libraryList, &QTreeWidget::itemDoubleClicked, [this](QTreeWidgetItem *item, int column)
	{
		// Fetch all tracks in list
		auto spotify = mainWindow->getSpotify();
		auto tracks =
			item->text(0) == "Recently Played"
			? spotify->recentlyPlayed()
			: item->text(0) == "Liked"
			  ? spotify->savedTracks()
			  : item->text(0) == "Tracks"
				? spotify->topTracks()
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
		auto status = spotify->playTracks(trackIds.first(), trackIds);
		if (!status.isEmpty())
			mainWindow->setStatus(QString("Failed to start playback: %1").arg(status), true);
	});

	// When expanding top artists, update it
	QTreeWidget::connect(libraryList, &QTreeWidget::itemExpanded, [this](QTreeWidgetItem *item)
	{
		QVector<QVariantList> results;
		item->takeChildren();

		if (item->text(0) == "Artists")
			for (auto &artist : mainWindow->getSpotify()->topArtists())
				results.append({artist.name, artist.id, Utils::RoleArtistId});
		else if (item->text(0) == "Albums")
			for (auto &album : mainWindow->getSpotify()->savedAlbums())
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
	});

	auto library = Utils::createGroupBox(QVector<QWidget*>() << libraryList, this);
	library->setTitle("Library");
	sidebar->addWidget(library);

	// Update current playlists
	mainWindow->refreshPlaylists();

	// Set default selected playlist
	playlists->setCurrentRow(0);
	QListWidget::connect(playlists, &QListWidget::itemClicked, this, [this](QListWidgetItem *item)
	{
		if (item != nullptr)
			libraryList->setCurrentItem(nullptr);
		auto currentPlaylist = mainWindow->getSptPlaylists().at(playlists->currentRow());
		mainWindow->loadPlaylist(currentPlaylist);
	});

	QListWidget::connect(playlists, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item)
	{
		auto currentPlaylist = mainWindow->getSptPlaylists().at(playlists->currentRow());
		mainWindow->loadPlaylist(currentPlaylist);
		auto result = mainWindow->getSpotify()->playTracks(
			QString("spotify:playlist:%1").arg(currentPlaylist.id));
		if (!result.isEmpty())
			mainWindow->setStatus(QString("Failed to start playlist playback: %1").arg(result), true);
	});

	playlists->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(playlists, &QWidget::customContextMenuRequested, [this](const QPoint &pos)
	{
		(new PlaylistMenu(
			*mainWindow->getSpotify(),
			mainWindow->getSptPlaylists().at(playlists->currentRow()), this))
			->popup(playlists->mapToGlobal(pos));
	});

	auto playlistContainer = Utils::createGroupBox(QVector<QWidget*>() << playlists, this);
	playlistContainer->setTitle("Playlists");
	sidebar->addWidget(playlistContainer);

	// Now playing song
	auto nowPlayingLayout = new QHBoxLayout();
	nowPlayingLayout->setSpacing(12);
	nowAlbum = new QLabel(this);
	nowAlbum->setFixedSize(64, 64);
	nowAlbum->setPixmap(Icon::get("media-optical-audio").pixmap(nowAlbum->size()));
	nowPlayingLayout->addWidget(nowAlbum);
	nowPlaying = new QLabel("No music playing", this);
	nowPlaying->setWordWrap(true);
	nowPlayingLayout->addWidget(nowPlaying);
	sidebar->addLayout(nowPlayingLayout);

	// Show menu when clicking now playing
	nowPlaying->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(nowPlaying, &QWidget::customContextMenuRequested, [this](const QPoint &pos)
	{
		auto track = mainWindow->getCurrentPlayback().item;
		mainWindow->songMenu(track.id, track.artist, track.name, track.artistId, track.albumId)
			->popup(nowPlaying->mapToGlobal(pos));
	});

	// Sidebar as widget
	auto sidebarWidget = Utils::layoutToWidget(sidebar);
	sidebarWidget->setMaximumWidth(250);
	addWidget(sidebarWidget);

	// Table with songs
	songs = new QTreeWidget(this);
	songs->setEditTriggers(QAbstractItemView::NoEditTriggers);
	songs->setSelectionBehavior(QAbstractItemView::SelectRows);
	songs->setSortingEnabled(true);
	songs->setRootIsDecorated(false);
	songs->setAllColumnsShowFocus(true);
	songs->setColumnCount(5);
	songs->setHeaderLabels(
		{
			" ", "Title", "Artist", "Album", "Length", "Added"
		}
	);
	songs->header()->setSectionsMovable(false);
	songs->header()->setSectionResizeMode((QHeaderView::ResizeMode) mainWindow->getSettings().general.songHeaderResizeMode);
	if (mainWindow->getSettings().general.songHeaderSortBy > 0)
		songs->header()->setSortIndicator(
			mainWindow->getSettings().general.songHeaderSortBy + 1,
			Qt::AscendingOrder);

	// Hide specified columns
	for (auto &value : mainWindow->getSettings().general.hiddenSongHeaders)
		songs->header()->setSectionHidden(value + 1, true);

	// Song context menu
	songs->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(songs, &QWidget::customContextMenuRequested, [this](const QPoint &pos)
	{
		auto item = songs->itemAt(pos);
		auto trackId = item->data(0, Utils::RoleTrackId).toString();
		if (trackId.isEmpty())
			return;
		mainWindow->songMenu(
				trackId,
				item->text(2),
				item->text(1),
				item->data(0, Utils::RoleArtistId).toString(),
				item->data(0, Utils::RoleAlbumId).toString())
			->popup(songs->mapToGlobal(pos));
	});

	QTreeWidget::connect(songs, &QTreeWidget::itemClicked, this, [=](QTreeWidgetItem *item, int column) {
		auto trackId = item->data(0, Utils::RoleTrackId).toString();
		if (trackId.isEmpty())
		{
			mainWindow->setStatus("Failed to start playback: track not found", true);
			return;
		}

		// If we played from library, we don't have any context
		auto allTracks = mainWindow->currentTracks();
		auto status =
			(libraryList->currentItem() != nullptr
				|| !mainWindow->getSettings().general.spotifyPlaybackOrder)
				&& allTracks.count() < 500
			? mainWindow->getSpotify()->playTracks(trackId, allTracks)
			: mainWindow->getSpotify()->playTracks(trackId, mainWindow->getSptContext());

		if (!status.isEmpty())
			mainWindow->setStatus(QString("Failed to start playback: %1").arg(status), true);
		else
			mainWindow->setPlayingTrackItem(item);

		mainWindow->refresh();
	});

	// Songs header context menu
	songs->header()->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(songs->header(), &QWidget::customContextMenuRequested, [this](const QPoint &pos) {
		auto menu = new QMenu(songs->header());
		auto showHeaders = menu->addMenu(Icon::get("visibility"), "Columns to show");
		auto sortBy = menu->addMenu(Icon::get("view-sort-ascending"), "Default sorting");
		auto settings = &mainWindow->getSettings();
		auto headerTitles = QStringList(
			{
				"Title", "Artist", "Album", "Length", "Added"
			}
		);
		auto headers = settings->general.hiddenSongHeaders;
		for (int i = 0; i < headerTitles.size(); i++)
		{
			auto showTitle = showHeaders->addAction(headerTitles.at(i));
			showTitle->setCheckable(true);
			showTitle->setChecked(std::find(headers.begin(), headers.end(), i) == headers.end());
			showTitle->setData(QVariant(i));

			auto sortTitle = sortBy->addAction(headerTitles.at(i));
			sortTitle->setCheckable(true);
			sortTitle->setChecked(i == settings->general.songHeaderSortBy);
			sortTitle->setData(QVariant(100 + i));
		}
		QMenu::connect(menu, &QMenu::triggered, [this](QAction *action) {
			int i = action->data().toInt();
			auto settings = &mainWindow->getSettings();
			// Columns to show
			if (i < 100)
			{
				songs->header()->setSectionHidden(i + 1, !action->isChecked());
				if (action->isChecked())
					settings->general.hiddenSongHeaders.erase(
						std::remove(
							settings->general.hiddenSongHeaders.begin(),
							settings->general.hiddenSongHeaders.end(), i));
				else
					settings->general.hiddenSongHeaders.push_back(i);
				settings->save();
				return;
			}
			// Sort by
			i -= 100;
			if (mainWindow->getSettings().general.songHeaderSortBy != i)
				songs->header()->setSortIndicator(i + 1, Qt::AscendingOrder);
			mainWindow->getSettings().general.songHeaderSortBy = mainWindow->getSettings().general.songHeaderSortBy == i ? -1 : i;
			mainWindow->getSettings().save();
		});
		menu->popup(songs->header()->mapToGlobal(pos));
	});

	// Load tracks in playlist
	auto playlistId = mainWindow->getSettings().general.lastPlaylist;
	if (mainWindow->getSptPlaylists().isEmpty())
	{
		// If no playlists were found
		// TODO: Load something from library here
	}
	else if (playlistId.isEmpty())
	{
		// Default to first in list
		playlistId = mainWindow->getSptPlaylists().at(0).id;
	}
	else
	{
		// Find playlist in list
		int i = 0;
		for (auto &playlist : mainWindow->getSptPlaylists())
		{
			if (playlist.id == playlistId)
			{
				playlists->setCurrentRow(i);
				mainWindow->loadPlaylist(playlist);
			}
			i++;
		}
	}
	// Add to main thing
	addWidget(songs);
}

void CentralWidget::refreshPlaylists()
{
	auto lastIndex =
		playlists == nullptr
		? -1
		: playlists->currentRow();
	mainWindow->getSptPlaylists() = spotify.playlists();

	// Add all playlists
	playlists->clear();
	for (auto &playlist : mainWindow->getSptPlaylists())
	{
		auto item = new QListWidgetItem(playlist.name, playlists);
		item->setToolTip(playlist.description);
		item->setData(Utils::RolePlaylistId, playlist.id);
	}
	if (lastIndex >= 0)
		playlists->setCurrentRow(lastIndex);
}