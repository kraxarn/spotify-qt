#include "searchview.hpp"

SearchView::SearchView(spt::Spotify &spotify, const lib::settings &settings, lib::cache &cache,
	QWidget *parent)
	: spotify(spotify),
	cache(cache),
	QWidget(parent)
{
	auto layout = new QVBoxLayout();
	layout->setContentsMargins(-1, 0, -1, 0);
	searchBox = new QLineEdit(this);
	layout->addWidget(searchBox);
	setLayout(layout);

	// Tabs
	tabs = new QTabWidget(this);
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
	QLineEdit::connect(searchBox, &QLineEdit::returnPressed, this, &SearchView::search);

	// Open album
	QTreeWidget::connect(albumList, &QTreeWidget::itemClicked, this, &SearchView::albumClick);
	// Album context menu
	albumList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(albumList, &QWidget::customContextMenuRequested, this, &SearchView::albumMenu);

	// Open artist
	QListWidget::connect(artistList, &QListWidget::itemClicked, this, &SearchView::artistClick);

	// Open playlist
	QListWidget::connect(playlistList, &QListWidget::itemClicked, this, &SearchView::playlistClick);

	// Open track
	QTreeWidget::connect(trackList, &QTreeWidget::itemActivated, this, &SearchView::trackClick);

	// Track context menu
	trackList->setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(trackList, &QWidget::customContextMenuRequested, this, &SearchView::trackMenu);

	// Playlist context menu
	playlistList->setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(playlistList, &QWidget::customContextMenuRequested,
		this, &SearchView::playlistMenu);
}

void SearchView::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);
	MainWindow::find(parentWidget())->getSearchAction()->setChecked(true);
}

void SearchView::hideEvent(QHideEvent *event)
{
	QWidget::hideEvent(event);
	MainWindow::find(parentWidget())->getSearchAction()->setChecked(false);
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

	auto albumMenu = new AlbumMenu(spotify, albumId.toStdString(), parentWidget());
	albumMenu->popup(albumList->mapToGlobal(pos));
}

void SearchView::albumClick(QTreeWidgetItem *item, int)
{
	auto mainWindow = MainWindow::find(parentWidget());
	if (!mainWindow->loadAlbum(item->data(0, RoleAlbumId)
		.toString().toStdString()))
	{
		mainWindow->setStatus(QString("Failed to load album"), true);
	}
}

void SearchView::artistClick(QListWidgetItem *item)
{
	auto mainWindow = MainWindow::find(parentWidget());
	mainWindow->openArtist(item->data(RoleArtistId).toString().toStdString());
	close();
}

void SearchView::playlistClick(QListWidgetItem *item)
{
	lib::spt::playlist playlist(spotify.playlist(item->data(RolePlaylistId)
		.toString().toStdString()));
	auto mainWindow = MainWindow::find(parentWidget());
	mainWindow->getSongsTree()->load(playlist);
}

void SearchView::search()
{
	// Empty all previous results
	trackList->clear();
	artistList->clear();
	albumList->clear();
	playlistList->clear();

	// Disable search box while searching
	searchBox->setEnabled(false);

	// Check if spotify uri
	auto searchText = searchBox->text();

	if (searchText.startsWith("spotify:")
		|| searchText.startsWith("https://open.spotify.com/"))
	{
		auto parts = searchText.startsWith("https://open.spotify.com/")
			? searchText.right(searchText.length() - 8).split('/')
			: searchText.split(':');

		if (parts.length() >= 3)
		{
			auto i = tabs->currentIndex();
			auto cat = parts[1];
			auto id = parts[2].toStdString();

			if (cat == "track")
			{
				addTrack(spotify.getTrack(id));
				i = 0;
			}
			else if (cat == "artist")
			{
				spotify.artist(id, [this](const lib::spt::artist &artist)
				{
					this->addArtist(artist);
				});
				i = 1;
			}
			else if (cat == "album")
			{
				spotify.getAlbum(id, [this](const lib::spt::album &album)
				{
					this->addAlbum(album);
				});
				i = 2;
			}
			else if (cat == "playlist")
			{
				addPlaylist(spotify.playlist(id));
				i = 3;
			}

			tabs->setCurrentIndex(i);
		}
	}
	else
	{
		// Get search results
		spotify.search(searchText.toStdString(),
			[this](const lib::spt::search_results &results)
			{
				this->resultsLoaded(results);
			});
	}
}

void SearchView::trackClick(QTreeWidgetItem *item, int)
{
	// Do we want it to continue playing results?
	auto trackId = lib::fmt::format("spotify:track:{}",
		item->data(0, RoleTrackId).toString().toStdString());

	spotify.play_tracks(0, {trackId}, [this](const std::string &status)
	{
		if (!status.empty())
		{
			auto mainWindow = MainWindow::find(this->parentWidget());
			mainWindow->status(lib::fmt::format("Failed to play track: {}",
				status), true);
		}
	});
}

void SearchView::trackMenu(const QPoint &pos)
{
	auto item = trackList->itemAt(pos);
	auto trackId = item->data(0, RoleTrackId).toString();
	if (trackId.isEmpty())
		return;
	(new SongMenu(item, spotify, parentWidget()))->popup(trackList->mapToGlobal(pos));
}

void SearchView::playlistMenu(const QPoint &pos)
{
	auto item = playlistList->itemAt(pos);
	auto playlistId = item->data(RolePlaylistId).toString().toStdString();
	auto menu = new PlaylistMenu(spotify, playlistId, cache, parentWidget());
	menu->popup(playlistList->mapToGlobal(pos));
}

void SearchView::addArtist(const lib::spt::artist &artist)
{
	auto name = QString::fromStdString(artist.name);
	auto id = QString::fromStdString(artist.id);

	auto item = new QListWidgetItem(name, artistList);
	item->setData(RoleArtistId, id);
	item->setToolTip(name);
}

void SearchView::addAlbum(const lib::spt::album &album)
{
	auto id = QString::fromStdString(album.id);
	auto name = QString::fromStdString(album.name);
	auto artist = QString::fromStdString(album.artist);

	auto item = new QTreeWidgetItem({
		name, artist
	});

	auto mainWindow = MainWindow::find(parentWidget());
	item->setIcon(0, mainWindow->getAlbum(album.image));
	item->setData(0, RoleAlbumId, id);
	item->setToolTip(0, name);
	item->setToolTip(1, artist);
	albumList->addTopLevelItem(item);
}

void SearchView::addPlaylist(const lib::spt::playlist &playlist)
{
	auto playlistName = QString::fromStdString(playlist.name);
	auto playlistId = QString::fromStdString(playlist.id);

	auto item = new QListWidgetItem(playlistName, playlistList);
	item->setData(RolePlaylistId, playlistId);
	item->setToolTip(playlistName);
}

void SearchView::addTrack(const lib::spt::track &track)
{
	auto trackName = QString::fromStdString(track.name);
	auto trackArtist = QString::fromStdString(track.artist);
	auto item = new QTreeWidgetItem(trackList, {
		trackName, trackArtist
	});
	item->setData(0, RoleTrackId, QString::fromStdString(track.id));
	item->setData(0, RoleArtistId, QString::fromStdString(track.artist_id));
	item->setData(0, RoleAlbumId, QString::fromStdString(track.album_id));
	item->setToolTip(0, trackName);
	item->setToolTip(1, trackArtist);
}

void SearchView::resultsLoaded(const lib::spt::search_results &results)
{
	// Albums
	for (auto &album : results.albums)
		addAlbum(album);

	// Artists
	for (auto &artist : results.artists)
		addArtist(artist);

	// Playlists
	for (auto &playlist : results.playlists)
		addPlaylist(playlist);

	// Tracks
	for (auto &track : results.tracks)
		addTrack(track);

	// Search done
	searchBox->setEnabled(true);
}
