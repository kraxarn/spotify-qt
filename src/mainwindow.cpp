#include "mainwindow.h"

MainWindow::MainWindow(spt::Spotify *spotify, QApplication *app, QWidget *parent) : QMainWindow(parent)
{
	// Set Spotify
	this->spotify = spotify;
	// Setup main window
	setWindowTitle("spotify-qt");
	setWindowIcon(QIcon::fromTheme("spotify"));
	resize(1280, 720);

}

MainWindow::~MainWindow()
{
	delete	playlists;
	delete	songs;
	delete	nowPlaying;
	delete	position;
	delete	nowAlbum;
	delete	progress;
	delete	playPause;
	delete	spotify;
	delete	sptPlaylists;
	delete	current;
}

QGroupBox *createGroupBox(QVector<QWidget*> &widgets)
{
	auto group = new QGroupBox();
	QVBoxLayout layout;
	for (auto &widget : widgets)
		layout.addWidget(widget);
	group->setLayout(&layout);
	return group;
}

QWidget *layoutToWidget(QLayout *layout)
{
	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}

QWidget *MainWindow::createCentralWidget()
{
	auto container = new QSplitter();
	// Sidebar with playlists etc.
	QVBoxLayout sidebar;
	auto libraryList = new QListWidget();
	playlists = new QListWidget();
	// Library
	libraryList->addItems({
		"Made For You", "Recently Played", "Liked Songs", "Albums", "Artists"
	});
	QListWidget::connect(libraryList, &QListWidget::itemPressed, this, [=](QListWidgetItem *item) {
		if (item != nullptr) {
			playlists->setCurrentRow(-1);
		}
	});
	auto library = createGroupBox(QVector<QWidget*>() << libraryList);
	library->setTitle("Library");
	sidebar.addWidget(library);
	// Update current playlists
	refreshPlaylists();
	// Set default selected playlist
	playlists->setCurrentRow(0);
	QListWidget::connect(playlists, &QListWidget::itemPressed, this, [=](QListWidgetItem *item) {
		if (item != nullptr)
			libraryList->setCurrentRow(-1);
		auto currentPlaylist = sptPlaylists->at(playlists->currentRow());
		loadPlaylist(currentPlaylist);
	});
	auto playlistContainer = createGroupBox(QVector<QWidget*>() << playlists);
	playlistContainer->setTitle("Playlists");
	sidebar.addWidget(playlistContainer);
	// Now playing song
	QHBoxLayout nowPlayingLayout;
	nowPlayingLayout.setSpacing(12);
	nowAlbum = new QLabel();
	nowAlbum->setFixedSize(64, 64);
	nowAlbum->setPixmap(QIcon::fromTheme("media-optical-audio").pixmap(nowAlbum->size()));
	nowPlayingLayout.addWidget(nowAlbum);
	nowPlaying = new QLabel("No music playing");
	nowPlayingLayout.addWidget(nowPlaying);
	sidebar.addLayout(&nowPlayingLayout);
	// Sidebar as widget
	auto sidebarWidget = layoutToWidget(&sidebar);
	sidebarWidget->setMaximumWidth(250);
	// Table with songs
	songs = new QTreeWidget();
	songs->setEditTriggers(QAbstractItemView::NoEditTriggers);
	songs->setSelectionBehavior(QAbstractItemView::SelectRows);
	songs->setSortingEnabled(true);
	songs->setRootIsDecorated(false);
	songs->setAllColumnsShowFocus(true);
	songs->setColumnCount(5);
	songs->sortByColumn(1, Qt::SortOrder::AscendingOrder);
	songs->setHeaderLabels({
		" ", "Title", "Artist", "Album", "Length"
	});
	songs->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	QTreeWidget::connect(songs, &QTreeWidget::itemPressed, this, [=](QTreeWidgetItem *item, int column) {
		auto trackId = item->data(0, 0x0100).toString();
		if (trackId.isEmpty())
		{
			setStatus("Failed to start playback: track not found");
			return;
		}
		// This is done in another thread in Go
		spotify->setShuffle(false);
		spotify->playTracks(*getTracksAfter(trackId));
	});
	// Load tracks in playlist
	auto currentPlaylist = sptPlaylists->at(playlists->currentRow());
	loadPlaylist(currentPlaylist);
	// Add to main thing
	container->addWidget(songs);
	return container;
}

void MainWindow::refreshPlaylists()
{
	spotify->playlists(sptPlaylists);
	// Create or empty
	if (playlists == nullptr)
		playlists = new QListWidget();
	else
		playlists->clear();
	// Add all playlists
	for (auto &playlist : *sptPlaylists)
		playlists->addItem(playlist.name);
}

bool MainWindow::loadPlaylist(spt::Playlist &playlist)
{
	auto tracks = playlist.loadTracks(spotify);
	songs->clear();
	for (int i = 0; i < tracks.length(); i++)
	{
		auto track = tracks.at(i);
		auto duration = QTime().addMSecs(track.duration());
		QTreeWidgetItem item({
			"", track.name(), track.artist(), track.album(),
			QString("%1:%2").arg(duration.minute()).arg(duration.second() % 60)
		});
		item.setData(0, 0x0100, QString("spotify:track:%1").arg(track.id()));
		if (track.isLocal)
			item.setDisabled(true);
		songs->insertTopLevelItem(i, &item);
	}
	return true;
}