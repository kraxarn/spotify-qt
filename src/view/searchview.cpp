#include "searchview.hpp"

SearchView::SearchView(spt::Spotify &spotify, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	httpClient(httpClient),
	QWidget(parent)
{
	auto *layout = new QVBoxLayout();
	layout->setContentsMargins(-1, 0, -1, 0);
	searchBox = new QLineEdit(this);
	layout->addWidget(searchBox);
	setLayout(layout);

	// Tab container
	tabs = new QTabWidget(this);
	layout->addWidget(tabs);

	// Tab content
	artists = new SearchTab::Artists(this);
	playlistList = new QListWidget(this);
	tracks = new SearchTab::Tracks(spotify, cache, this);
	albumList = defaultTree({
		"Title", "Artist"
	});

	// Add all tabs
	tabs->addTab(tracks, "Tracks");
	tabs->addTab(artists, "Artists");
	tabs->addTab(albumList, "Albums");
	tabs->addTab(playlistList, "Playlists");

	// Start searching when pressing enter
	QLineEdit::connect(searchBox, &QLineEdit::returnPressed,
		this, &SearchView::search);

	// Open album
	QTreeWidget::connect(albumList, &QTreeWidget::itemClicked,
		this, &SearchView::albumClick);
	// Album context menu
	albumList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(albumList, &QWidget::customContextMenuRequested,
		this, &SearchView::albumMenu);

	// Open playlist
	QListWidget::connect(playlistList, &QListWidget::itemClicked,
		this, &SearchView::playlistClick);

	// Playlist context menu
	playlistList->setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(playlistList, &QWidget::customContextMenuRequested,
		this, &SearchView::playlistMenu);
}

void SearchView::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);
	MainWindow::find(parentWidget())->setSearchChecked(true);
	searchBox->setFocus();
}

void SearchView::hideEvent(QHideEvent *event)
{
	QWidget::hideEvent(event);
	MainWindow::find(parentWidget())->setSearchChecked(false);
}

auto SearchView::defaultTree(const QStringList &headers) -> QTreeWidget *
{
	auto *tree = new QTreeWidget(this);
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
	auto *item = albumList->itemAt(pos);
	const auto &albumId = item->data(0, RoleAlbumId).toString();
	if (albumId.isEmpty())
	{
		return;
	}

	auto *albumMenu = new AlbumMenu(spotify, cache, albumId.toStdString(),
		parentWidget());
	albumMenu->popup(albumList->mapToGlobal(pos));
}

void SearchView::albumClick(QTreeWidgetItem *item, int /*column*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	if (!mainWindow->loadAlbum(item->data(0, RoleAlbumId)
		.toString().toStdString()))
	{
		mainWindow->setStatus(QString("Failed to load album"), true);
	}
}

void SearchView::playlistClick(QListWidgetItem *item)
{
	spotify.playlist(item->data(RolePlaylistId)
		.toString().toStdString(), [this](const lib::spt::playlist &playlist)
	{
		auto *mainWindow = MainWindow::find(this->parentWidget());
		mainWindow->getSongsTree()->load(playlist);
	});
}

void SearchView::search()
{
	// Empty all previous results
	tracks->clear();
	artists->clear();
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
				spotify.track(id, [this](const lib::spt::track &track)
				{
					this->tracks->add(track);
				});
				i = 0;
			}
			else if (cat == "artist")
			{
				spotify.artist(id, [this](const lib::spt::artist &artist)
				{
					this->artists->add(artist);
				});
				i = 1;
			}
			else if (cat == "album")
			{
				spotify.album(id, [this](const lib::spt::album &album)
				{
					this->addAlbum(album);
				});
				i = 2;
			}
			else if (cat == "playlist")
			{
				spotify.playlist(id, [this](const lib::spt::playlist &playlist)
				{
					this->addPlaylist(playlist);
				});
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

void SearchView::playlistMenu(const QPoint &pos)
{
	auto *item = playlistList->itemAt(pos);
	auto playlistId = item->data(RolePlaylistId).toString().toStdString();

	spotify.playlist(playlistId, [this, pos](const lib::spt::playlist &playlist)
	{
		auto *menu = new PlaylistMenu(spotify, playlist, cache, parentWidget());
		menu->popup(playlistList->mapToGlobal(pos));
	});
}

void SearchView::addAlbum(const lib::spt::album &album)
{
	auto id = QString::fromStdString(album.id);
	auto name = QString::fromStdString(album.name);
	auto artist = QString::fromStdString(album.artist);

	auto *item = new QTreeWidgetItem({
		name, artist
	});

	HttpUtils::getAlbum(album.image, httpClient, cache, [item](const QPixmap &image)
	{
		if (item != nullptr)
		{
			item->setIcon(0, image);
		}
	});

	item->setData(0, RoleAlbumId, id);
	item->setToolTip(0, name);
	item->setToolTip(1, artist);
	albumList->addTopLevelItem(item);
}

void SearchView::addPlaylist(const lib::spt::playlist &playlist)
{
	auto playlistName = QString::fromStdString(playlist.name);
	auto playlistId = QString::fromStdString(playlist.id);

	auto *item = new QListWidgetItem(playlistName, playlistList);
	item->setData(RolePlaylistId, playlistId);
	item->setToolTip(playlistName);
}

void SearchView::resultsLoaded(const lib::spt::search_results &results)
{
	// Albums
	for (const auto &album : results.albums)
	{
		addAlbum(album);
	}

	// Artists
	for (const auto &artist : results.artists)
	{
		this->artists->add(artist);
	}

	// Playlists
	for (const auto &playlist : results.playlists)
	{
		addPlaylist(playlist);
	}

	// Tracks
	for (const auto &track : results.tracks)
	{
		tracks->add(track);
	}

	// Search done
	searchBox->setEnabled(true);
}
