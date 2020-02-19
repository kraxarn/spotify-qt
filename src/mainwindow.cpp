#include "mainwindow.hpp"

MainWindow::MainWindow(spt::Spotify *spotify, QApplication *app, QWidget *parent) : QMainWindow(parent)
{
	// Set Spotify
	this->spotify = spotify;
	// Setup main window
	setWindowTitle("spotify-qt");
	setWindowIcon(QIcon::fromTheme("spotify"));
	resize(1280, 720);
	setCentralWidget(createCentralWidget());
	addToolBar(Qt::ToolBarArea::TopToolBarArea, createToolBar());
	// Update player status
	auto timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout, this, [=]() {
		current = spotify->currentPlayback();
		if (!current.isPlaying)
		{
			playPause->setIcon(QIcon::fromTheme("media-playback-start"));
			playPause->setText("Play");
			return;
		}
		auto currPlaying = QString("%1\n%2").arg(current.item->name()).arg(current.item->artist());
		if (nowPlaying->text() != currPlaying)
		{
			if (nowPlaying->text() != "No music playing")
				setCurrentSongIcon();
			nowPlaying->setText(currPlaying);
			setAlbumImage(current.item->image());
		}
		position->setText(QString("%1/%2")
			.arg(formatTime(current.progressMs))
			.arg(formatTime(current.item->duration())));
		progress->setValue(current.progressMs);
		progress->setMaximum(current.item->duration());
		playPause->setIcon(QIcon::fromTheme(
			current.isPlaying ? "media-playback-pause" : "media-playback-start"));
		playPause->setText(current.isPlaying ? "Pause" : "Play");
	});
	timer->start(1000);
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
	delete	sptPlaylists;
}

QGroupBox *createGroupBox(QVector<QWidget*> &widgets)
{
	auto group = new QGroupBox();
	auto layout = new QVBoxLayout();
	for (auto &widget : widgets)
		layout->addWidget(widget);
	group->setLayout(layout);
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
	auto sidebar = new QVBoxLayout();
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
	sidebar->addWidget(library);
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
	sidebar->addWidget(playlistContainer);
	// Now playing song
	auto nowPlayingLayout = new QHBoxLayout();
	nowPlayingLayout->setSpacing(12);
	nowAlbum = new QLabel();
	nowAlbum->setFixedSize(64, 64);
	nowAlbum->setPixmap(QIcon::fromTheme("media-optical-audio").pixmap(nowAlbum->size()));
	nowPlayingLayout->addWidget(nowAlbum);
	nowPlaying = new QLabel("No music playing");
	nowPlayingLayout->addWidget(nowPlaying);
	sidebar->addLayout(nowPlayingLayout);
	// Sidebar as widget
	auto sidebarWidget = layoutToWidget(sidebar);
	sidebarWidget->setMaximumWidth(250);
	container->addWidget(sidebarWidget);
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

QToolBar *MainWindow::createToolBar()
{
	auto toolBar = new QToolBar();
	toolBar->setMovable(false);
	// Menu
	auto menu = new QToolButton();
	menu->setText("Menu");
	menu->setIcon(QIcon::fromTheme("application-menu"));
	menu->setPopupMode(QToolButton::InstantPopup);
	menu->setMenu(createMenu());
	toolBar->addWidget(menu);
	toolBar->addSeparator();
	// Media controls
	toolBar->addAction(QIcon::fromTheme("media-skip-backward"), "Previous");
	playPause = toolBar->addAction(QIcon::fromTheme("media-playback-start"), "Play");
	QAction::connect(playPause, &QAction::triggered, [=](bool checked) {
		if (playPause->iconText() == "Pause")
			spotify->pause();
		else
			spotify->resume();
	});
	toolBar->addAction(QIcon::fromTheme("media-playback-stop"), "Stop");
	toolBar->addAction(QIcon::fromTheme("media-skip-forward"),  "Next");
	// Progress
	progress = new QSlider();
	progress->setOrientation(Qt::Orientation::Horizontal);
	toolBar->addSeparator();
	toolBar->addWidget(progress);
	toolBar->addSeparator();
	position = new QLabel("0:00/0:00");
	toolBar->addWidget(position);
	toolBar->addSeparator();
	// Repeat and shuffle toggles
	toolBar->addAction(QIcon::fromTheme("media-playlist-repeat"), "Repeat")
		->setCheckable(true);
	toolBar->addAction(QIcon::fromTheme("media-playlist-shuffle"), "Shuffle")
		->setCheckable(true);
	// Volume slider
	QSlider volume;
	volume.setOrientation(Qt::Orientation::Horizontal);
	volume.setMaximumWidth(100);
	volume.setMinimum(0);
	volume.setMaximum(20);
	volume.setValue(20);
	toolBar->addWidget(&volume);
	// Return final tool bar
	return toolBar;
}

QAction *createMenuAction(QString icon, QString text, QKeySequence::StandardKey shortcut)
{
	auto action = new QAction(QIcon::fromTheme(icon), text);
	if (shortcut != QKeySequence::UnknownKey)
		action->setShortcut(QKeySequence(shortcut));
	return action;
}

QMenu *MainWindow::createMenu()
{
	// Create root
	auto menu = new QMenu();
	// About
	auto aboutMenu = new QMenu("About");
	aboutMenu->setIcon(QIcon::fromTheme("help-about"));
	aboutMenu->addActions({
		createMenuAction("spotify", "About spotify-qt", QKeySequence::UnknownKey),
		createMenuAction("qt",      "About Qt",         QKeySequence::UnknownKey)
	});
	// Check for updates
	aboutMenu->addSeparator();
	aboutMenu->addActions({
		createMenuAction("download",  "Check for updates", QKeySequence::UnknownKey)
	});
	menu->addMenu(aboutMenu);
	// Device selection
	auto deviceMenu = new QMenu("Device");
	deviceMenu->setIcon(QIcon::fromTheme("speaker"));
	auto deviceRefresh = new QAction(QIcon::fromTheme("reload"), "Refresh");
	QAction::connect(deviceRefresh, &QAction::triggered, [=](bool checked) {
		// Set status and get devices
		setStatus("Refreshing devices...");
		auto devices = spotify->devices();
		if (devices.isEmpty())
		{
			setStatus("No devices found");
			return;
		}
		// Clear all entries
		if (deviceMenu->actions().length() > 2)
		{
			for (int i = 0; i < deviceMenu->actions().size(); i++)
			{
				if (i < 2)
					continue;
				deviceMenu->removeAction(deviceMenu->actions().at(i));
			}
		}
		// Update devices
		setStatus(QString("Found %1 device(s)").arg(devices.length()));
		for (auto &device : devices)
		{
			auto action = deviceMenu->addAction(device.name);
			action->setCheckable(true);
			action->setChecked(device.isActive);
			action->setDisabled(device.isActive);
			QAction::connect(action, &QAction::triggered, [=](bool triggered) {
				if (!spotify->setDevice(device))
				{
					action->setChecked(false);
					setStatus(QString("Failed to set device"));
				}
				else
					action->setDisabled(true);
			});
		}
	});
	deviceMenu->addActions({
		deviceRefresh
	});
	deviceMenu->addSeparator();
	menu->addMenu(deviceMenu);
	// Refresh and settings
	menu->addActions({
		createMenuAction("settings", "Settings...", QKeySequence::Preferences)
	});
	// Log out and quit
	menu->addSeparator();
	menu->addActions({
		createMenuAction("im-user-away",     "Log out", QKeySequence::UnknownKey),
		createMenuAction("application-exit", "Quit",    QKeySequence::Quit)
	});
	// Return final menu
	return menu;
}

void MainWindow::refreshPlaylists()
{
	spotify->playlists(&sptPlaylists);
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
	auto tracks = playlist.loadTracks(*spotify);
	songs->clear();
	for (int i = 0; i < tracks->length(); i++)
	{
		auto track = tracks->at(i);
		auto item = new QTreeWidgetItem({
			"", track.name(), track.artist(), track.album(), formatTime(track.duration())
		});
		item->setData(0, 0x0100, QString("spotify:track:%1").arg(track.id()));
		if (track.isLocal)
			item->setDisabled(true);
		songs->insertTopLevelItem(i, item);
	}
	return true;
}

void MainWindow::setStatus(QString message)
{
	statusBar()->showMessage(message, 3000);
}

QStringList *MainWindow::getTracksAfter(QString &trackId)
{
	auto tracks = new QStringList();
	auto found = false;
	for (int i = 0; i < songs->topLevelItemCount(); i++)
	{
		auto item = songs->topLevelItem(i)->data(0, 0x0100).toString();
		if (!found && item == trackId)
			found = true;
		if (!found)
			continue;
		tracks->append(item);
	}
	return tracks;
}

void MainWindow::setCurrentSongIcon()
{
	for (int i = 0; i < songs->topLevelItemCount(); i++)
	{
		auto item = songs->topLevelItem(i);
		if (item->data(0, 0x0100).toString() == QString("spotify:track:%1").arg(current.item->id()))
			item->setIcon(0, QIcon::fromTheme("media-playback-start"));
		else
			item->setIcon(0, QIcon());
	}
}

void MainWindow::setAlbumImage(QString url)
{
	if (network == nullptr)
		network = new QNetworkAccessManager();
	auto reply = network->get(QNetworkRequest(QUrl(url)));
	while (!reply->isFinished())
		QCoreApplication::processEvents();
	auto art = new QPixmap();
	art->loadFromData(reply->readAll(), "jpeg");
	nowAlbum->setPixmap(art->scaled(64, 64));
}

QString MainWindow::formatTime(int ms)
{
	auto duration = QTime(0, 0).addMSecs(ms);
	return QString("%1:%2")
		.arg(duration.minute())
		.arg(duration.second() % 60, 2, 10, QChar('0'));
}
