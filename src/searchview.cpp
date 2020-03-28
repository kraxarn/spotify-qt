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
		searchBox->setEnabled(true);
		// Albums
		for (auto &album : results.albums)
		{
			auto item = new QTreeWidgetItem({
				album.name, album.artist
			});
			item->setIcon(0, window->getAlbum(album.image));
			item->setData(0, MainWindow::RoleAlbumId, album.id);
			albumList->addTopLevelItem(item);
		}
		// Artists
		for (auto &artist : results.artists)
		{
			auto item = new QListWidgetItem(artist.name, artistList);
			item->setData(MainWindow::RoleArtistId, artist.id);
		}
		// Playlists
		for (auto &playlist : results.playlists)
			playlistList->addItem(playlist.name);
		// Tracks
		for (auto &track : results.tracks)
			trackList->addTopLevelItem(new QTreeWidgetItem({
				track.name, track.artist
			}));

	});

	// Open artist view
	QListWidget::connect(artistList, &QListWidget::itemClicked, [this, window](QListWidgetItem *item) {
		artistList->setEnabled(false);
		window->openArtist(item->data(MainWindow::RoleArtistId).toString());
		close();
	});
	// Open album
	QTreeWidget::connect(albumList, &QTreeWidget::itemClicked, [this, window](QTreeWidgetItem *item, int column) {
		if (!window->loadAlbum(item->data(0, MainWindow::RoleAlbumId).toString(), false))
			window->setStatus(QString("Failed to load album"));
	});

	// Setup dock
	setWindowTitle("Search");
	setWidget(MainWindow::layoutToWidget(layout));
	setFixedWidth(320);
	// Uncheck search when closing
	QDockWidget::connect(this, &QDockWidget::visibilityChanged, [window](bool visible) {
		window->search->setChecked(visible);
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