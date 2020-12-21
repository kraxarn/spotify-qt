#include "searchview.hpp"

SearchView::SearchView(spt::Spotify &spotify, const Settings &settings, QWidget *parent)
	: spotify(spotify), parent(parent), QWidget(parent)
{
	auto window = (MainWindow *) parent;
	auto layout = new QVBoxLayout();
	layout->setContentsMargins(-1, 0, -1, 0);
	auto searchBox = new QLineEdit(this);
	layout->addWidget(searchBox);
	setLayout(layout);

	// Tabs
	auto tabs = new QTabWidget(this);
	layout->addWidget(tabs);

	// All lists
	artistList = new QListWidget(this);
	playlistList = new QListWidget(this);

	// Track list
	trackList = defaultTree({
		"Title", "Artist"
	});

	// Album list
	albumList = defaultTree({
		"Title", "Artist"
	});

	// Add all tabs
	tabs->addTab(trackList, "Tracks");
	tabs->addTab(artistList, "Artists");
	tabs->addTab(albumList, "Albums");
	tabs->addTab(playlistList, "Playlists");

	// Start searching when pressing enter
	QLineEdit::connect(searchBox, &QLineEdit::returnPressed, [this, &spotify, searchBox, window]()
	{
		// Empty all previous results
		trackList->clear();
		artistList->clear();
		albumList->clear();
		playlistList->clear();

		// Get new results
		searchBox->setEnabled(false);
		auto results = spotify.search(searchBox->text());

		// Albums
		for (auto &album : results.albums)
		{
			auto item = new QTreeWidgetItem({
				album.name, album.artist
			});
			item->setIcon(0, window->getAlbum(album.image));
			item->setData(0, RoleAlbumId, album.id);
			albumList->addTopLevelItem(item);
		}

		// Artists
		for (auto &artist : results.artists)
		{
			auto item = new QListWidgetItem(artist.name, artistList);
			item->setData(RoleArtistId, artist.id);
		}

		// Playlists
		for (auto &json : results.playlists)
		{
			spt::Playlist playlist(json);
			auto item = new QListWidgetItem(playlist.name, playlistList);
			item->setData(RolePlaylistId, playlist.id);
		}

		// Tracks
		for (auto &track : results.tracks)
		{
			auto item = new QTreeWidgetItem(trackList, {
				track.name, track.artist
			});
			item->setData(0, RoleTrackId, track.id);
			item->setData(0, RoleArtistId, track.artistId);
			item->setData(0, RoleAlbumId, track.albumId);
		}

		// Search done
		searchBox->setEnabled(true);
	});

	// Open album
	QTreeWidget::connect(albumList, &QTreeWidget::itemClicked, [this, window](QTreeWidgetItem *item, int column)
	{
		if (!window->loadAlbum(item->data(0, RoleAlbumId).toString(), false))
			window->setStatus(QString("Failed to load album"), true);
	});

	// Album context menu
	albumList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(albumList, &QWidget::customContextMenuRequested, this, &SearchView::albumMenu);

	// Open artist
	QListWidget::connect(artistList, &QListWidget::itemClicked, [this, window](QListWidgetItem *item)
	{
		window->openArtist(item->data(RoleArtistId).toString());
		close();
	});

	// Open playlist
	QListWidget::connect(playlistList, &QListWidget::itemClicked, [this, window, &spotify](QListWidgetItem *item)
	{
		spt::Playlist playlist(spotify.playlist(item->data(RolePlaylistId).toString()));
		if (!window->loadPlaylist(playlist))
			window->setStatus(QString("Failed to load playlist"), true);
		else
			window->setCurrentPlaylistItem(-1);
	});

	// Open track
	QTreeWidget::connect(trackList, settings.general.singleClickPlay
			? &QTreeWidget::itemClicked : &QTreeWidget::itemDoubleClicked,
		[window, &spotify](QTreeWidgetItem *item, int column)
		{
			// Do we want it to continue playing results?
			auto trackId = QString("spotify:track:%1")
				.arg(item->data(0, RoleTrackId).toString());
			auto status = spotify.playTracks(0, QStringList(trackId));
			if (!status.isEmpty())
				window->setStatus(QString("Failed to play track: %1").arg(status), true);
		});

	// Track context menu
	trackList->setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(trackList, &QWidget::customContextMenuRequested, [this, window, &spotify](const QPoint &pos)
	{
		auto item = trackList->itemAt(pos);
		auto trackId = item->data(0, RoleTrackId).toString();
		if (trackId.isEmpty())
			return;
		(new SongMenu(item, spotify, window))->popup(trackList->mapToGlobal(pos));
	});

	// Playlist context menu
	playlistList->setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(playlistList, &QWidget::customContextMenuRequested, [this, window, &spotify](const QPoint &pos)
	{
		auto item = playlistList->itemAt(pos);
		(new PlaylistMenu(spotify, item->data(RolePlaylistId).toString(), window))
			->popup(playlistList->mapToGlobal(pos));
	});
}

void SearchView::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);
	dynamic_cast<MainWindow*>(parent)->getSearchAction()->setChecked(true);
}

void SearchView::hideEvent(QHideEvent *event)
{
	QWidget::hideEvent(event);
	dynamic_cast<MainWindow*>(parent)->getSearchAction()->setChecked(false);
}

QTreeWidget *SearchView::defaultTree(const QStringList &headers)
{
	auto tree = new QTreeWidget(this);
	tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tree->setSelectionBehavior(QAbstractItemView::SelectRows);
	tree->setRootIsDecorated(false);
	tree->setAllColumnsShowFocus(true);
	tree->setColumnCount(headers.length());
	tree->setHeaderLabels(headers);
	return tree;
}

void SearchView::albumMenu(const QPoint &pos)
{
	auto item = albumList->itemAt(pos);
	auto albumId = item->data(0, RoleAlbumId).toString();
	if (albumId.isEmpty())
		return;
	(new AlbumMenu(spotify, albumId, parent))->popup(albumList->mapToGlobal(pos));
}