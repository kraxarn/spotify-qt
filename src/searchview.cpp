#include "searchview.hpp"

SearchView::SearchView(spt::Spotify &spotify, QWidget *parent) : QDockWidget(parent)
{
	auto window = (MainWindow*) parent;
	auto layout = new QVBoxLayout();
	layout->setContentsMargins(-1, 0, -1, 0);
	auto searchBox = new QLineEdit(this);
	layout->addWidget(searchBox);
	// Tabs
	auto tabs = new QTabWidget(this);
	layout->addWidget(tabs);
	// All lists
	artistList		= new QListWidget(this);
	playlistList	= new QListWidget(this);
	// Track list
	trackList = defaultTree({
		"Title", "Artist"
	});
	// Album list
	albumList = defaultTree({
		"Title", "Artist"
	});
	// Add all tabs
	tabs->addTab(trackList,		"Tracks");
	tabs->addTab(artistList,	"Artists");
	tabs->addTab(albumList,		"Albums");
	tabs->addTab(playlistList,	"Playlists");

	// Start searching when pressing enter
	QLineEdit::connect(searchBox, &QLineEdit::returnPressed, [this, &spotify, searchBox, window]() {
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
			item->setData(0, Utils::RoleAlbumId, album.id);
			albumList->addTopLevelItem(item);
		}
		// Artists
		for (auto &artist : results.artists)
		{
			auto item = new QListWidgetItem(artist.name, artistList);
			item->setData(Utils::RoleArtistId, artist.id);
		}
		// Playlists
		for (auto &json : results.playlists)
		{
			spt::Playlist playlist(json);
			auto item = new QListWidgetItem(playlist.name, playlistList);
			item->setData(Utils::RolePlaylistId, playlist.id);
		}
		// Tracks
		for (auto &track : results.tracks)
		{
			auto item = new QTreeWidgetItem(trackList,{
				track.name, track.artist
			});
			item->setData(0, Utils::RoleTrackId, track.id);
			item->setData(0, Utils::RoleArtistId, track.artistId);
			item->setData(0, Utils::RoleAlbumId, track.albumId);
		}
		// Search done
		searchBox->setEnabled(true);
	});

	// Open album
	QTreeWidget::connect(albumList, &QTreeWidget::itemClicked, [this, window](QTreeWidgetItem *item, int column) {
		if (!window->loadAlbum(item->data(0, Utils::RoleAlbumId).toString(), false))
			window->setStatus(QString("Failed to load album"), true);
	});
	// Open artist
	QListWidget::connect(artistList, &QListWidget::itemClicked, [this, window](QListWidgetItem *item) {
		window->openArtist(item->data(Utils::RoleArtistId).toString());
		close();
	});
	// Open playlist
	QListWidget::connect(playlistList, &QListWidget::itemClicked, [this, window](QListWidgetItem *item) {
		spt::Playlist playlist(item->data(0x100).value<QJsonObject>());
		if (!window->loadPlaylist(playlist))
			window->setStatus(QString("Failed to load playlist"), true);
		else
			window->getPlaylistsList()->setCurrentRow(-1);
	});
	// Open track
	QTreeWidget::connect(trackList, &QTreeWidget::itemClicked, [this, window, &spotify](QTreeWidgetItem *item, int column) {
		// Do we want it to continue playing results?
		auto trackId = QString("spotify:track:%1")
			.arg(item->data(0, Utils::RoleTrackId).toString());
		auto status = spotify.playTracks(trackId, QStringList(trackId));
		if (!status.isEmpty())
			window->setStatus(QString("Failed to play track: %1").arg(status), true);
	});

	// Track context menu
	trackList->setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(trackList, &QWidget::customContextMenuRequested, [this, window, &spotify](const QPoint &pos) {
		auto item = trackList->itemAt(pos);
		auto trackId = item->data(0, Utils::RoleTrackId).toString();
		if (trackId.isEmpty())
			return;
		(new SongMenu(trackId, item->text(1),
			item->text(0), item->data(0, Utils::RoleArtistId).toString(),
			item->data(0, Utils::RoleAlbumId).toString(), &spotify, window))
			->popup(trackList->mapToGlobal(pos));
	});
	// Playlist context menu
	playlistList->setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(playlistList, &QWidget::customContextMenuRequested, [this, window, &spotify](const QPoint &pos) {
		auto item = playlistList->itemAt(pos);
		(new PlaylistMenu(spotify, item->data(Utils::RolePlaylistId).toString(), window))
			->popup(playlistList->mapToGlobal(pos));
	});

	// Setup dock
	setWindowTitle("Search");
	setWidget(Utils::layoutToWidget(layout));
	setFixedWidth(320);
	// Uncheck search when closing
	QDockWidget::connect(this, &QDockWidget::visibilityChanged, [window](bool visible) {
		window->getSearchAction()->setChecked(visible);
	});
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