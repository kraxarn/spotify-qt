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
	trackList		= new QTreeWidget(this);
	artistList		= new QListWidget(this);
	albumList		= new QListWidget(this);
	playlistList	= new QListWidget(this);
	// Track list
	trackList->setEditTriggers(QAbstractItemView::NoEditTriggers);
	trackList->setSelectionBehavior(QAbstractItemView::SelectRows);
	trackList->setRootIsDecorated(false);
	trackList->setAllColumnsShowFocus(true);
	trackList->setColumnCount(2);
	trackList->setHeaderLabels({
		"Title", "Artist"
	});
	trackList->header()->setSectionResizeMode(QHeaderView::Stretch);
	trackList->header()->setSectionsMovable(false);
	// Add all tabs
	tabs->addTab(trackList,		"Tracks");
	tabs->addTab(artistList,	"Artists");
	tabs->addTab(albumList,		"Albums");
	tabs->addTab(playlistList,	"Playlists");

	// Start searching when pressing enter
	QLineEdit::connect(searchBox, &QLineEdit::returnPressed, [this, &spotify, searchBox]() {
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
			albumList->addItem(album.name);
		// Artists
		for (auto &artist : results.artists)
			artistList->addItem(artist.name);
		// Playlists
		for (auto &playlist : results.playlists)
			playlistList->addItem(playlist.name);
		// Tracks
		for (auto &track : results.tracks)
			trackList->addTopLevelItem(new QTreeWidgetItem({
				track.name, track.artist
			}));
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