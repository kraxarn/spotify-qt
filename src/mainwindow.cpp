#include "mainwindow.hpp"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	// Some default values to prevent unexpected stuff
	playlists 	= nullptr;
	songs 		= nullptr;
	sptClient	= nullptr;
	volume 		= progress	= nullptr;
	nowPlaying	= position	= nowAlbum	= nullptr;
	repeat 		= shuffle	= playPause	= nullptr;
	// Set Spotify
	spotify = new spt::Spotify();
	sptPlaylists = new QVector<spt::Playlist>();
	network = new QNetworkAccessManager();
	// Setup main window
	setWindowTitle("spotify-qt");
	setWindowIcon(QIcon::fromTheme("spotify"));
	resize(1280, 720);
	setCentralWidget(createCentralWidget());
	addToolBar(Qt::ToolBarArea::TopToolBarArea, createToolBar());
	// Apply selected style
	Settings settings;
	QApplication::setStyle(settings.style());
	// Update player status
	auto timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout, this, &MainWindow::refresh);
	refresh();
	timer->start(1000);
	setStatus("Welcome to spotify-qt!");
	// Check if should start client
	if (settings.sptStartClient())
	{
		sptClient = new spt::ClientHandler();
		auto status = sptClient->start();
		if (!status.isEmpty())
			QMessageBox::warning(this,
				"Client error",
				QString("Failed to autostart Spotify client: %1").arg(status));
	}
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
	delete	spotify;
	delete sptClient;
}

void MainWindow::refresh()
{
	current = spotify->currentPlayback();
	if (!current.isPlaying)
	{
		playPause->setIcon(QIcon::fromTheme("media-playback-start"));
		playPause->setText("Play");
		return;
	}
	auto currPlaying = QString("%1\n%2").arg(current.item->name).arg(current.item->artist);
	if (nowPlaying->text() != currPlaying)
	{
		if (nowPlaying->text() != "No music playing")
			setCurrentSongIcon();
		nowPlaying->setText(currPlaying);
		setAlbumImage(current.item->image);
		setWindowTitle(QString("%1 - %2").arg(current.item->artist).arg(current.item->name));
	}
	position->setText(QString("%1/%2")
		.arg(formatTime(current.progressMs))
		.arg(formatTime(current.item->duration)));
	progress->setValue(current.progressMs);
	progress->setMaximum(current.item->duration);
	playPause->setIcon(QIcon::fromTheme(
		current.isPlaying ? "media-playback-pause" : "media-playback-start"));
	playPause->setText(current.isPlaying ? "Pause" : "Play");
	if (!Settings().pulseVolume())
		volume->setValue(current.volume / 5);
	repeat->setChecked(current.repeat != "off");
	shuffle->setChecked(current.shuffle);
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
	libraryList->setEnabled(false);
	libraryList->setToolTip("Not implemented yet");
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
	songs->setHeaderLabels({
		" ", "Title", "Artist", "Album", "Length"
	});
	songs->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	songs->header()->setSectionsMovable(false);
	// Song context menu
	songs->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(songs, &QWidget::customContextMenuRequested, [=](const QPoint &pos) {
		auto item = songs->itemAt(pos);
		auto trackId = item->data(0, RoleTrackId).toString();
		if (trackId.isEmpty())
			return;
		auto songMenu = new QMenu(songs);
		auto trackFeatures = songMenu->addAction(QIcon::fromTheme("view-statistics"), "Audio features");
		QAction::connect(trackFeatures, &QAction::triggered, [=](bool checked) {
			openAudioFeaturesWidget(trackId, item->text(2), item->text(1));
		});
		auto lyrics = songMenu->addAction(QIcon::fromTheme("view-media-lyrics"), "Lyrics");
		QAction::connect(lyrics, &QAction::triggered, [=](bool checked) {
			openLyrics(item->text(2), item->text(1));
		});
		auto share = songMenu->addMenu(QIcon::fromTheme("document-share"), "Share");
		auto shareSongLink = share->addAction("Copy song link");
		QAction::connect(shareSongLink, &QAction::triggered, [=](bool checked) {
			QApplication::clipboard()->setText(
				QString("https://open.spotify.com/track/%1")
					.arg(QString(trackId).remove(0, QString("spotify:track:").length())));
			setStatus("Link copied to clipboard");
		});
		songMenu->addSeparator();
		auto goArtist = songMenu->addAction(QIcon::fromTheme("view-media-artist"), "View artist");
		QAction::connect(goArtist, &QAction::triggered, [=](bool checked) {
			openArtist(item->data(0, RoleArtistId).toString());
		});
		auto goAlbum = songMenu->addAction(QIcon::fromTheme("view-media-album-cover"), "Open album");
		goAlbum->setEnabled(!sptContext.startsWith("spotify:album"));
		QAction::connect(goAlbum, &QAction::triggered, [=](bool checked) {
			auto albumId = item->data(0, RoleAlbumId).toString();
			auto tracks = spotify->albumTracks(albumId);
			if (tracks->length() <= 1)
				setStatus("Album only contains one song or is empty");
			else
			{
				playlists->setCurrentRow(-1);
				libraryList->setCurrentRow(-1);
				sptContext = QString("spotify:album:%1").arg(albumId);
				loadSongs(*tracks);
			}
			delete tracks;
		});
		songMenu->popup(songs->mapToGlobal(pos));
	});
	QTreeWidget::connect(songs, &QTreeWidget::itemClicked, this, [=](QTreeWidgetItem *item, int column) {
		auto trackId = item->data(0, RoleTrackId).toString();
		if (trackId.isEmpty())
		{
			setStatus("Failed to start playback: track not found");
			return;
		}
		auto status = spotify->playTracks(trackId, sptContext);
		if (!status.isEmpty())
			setStatus(QString("Failed to start playback: %1").arg(status));
		refresh();
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
	auto toolBar = new QToolBar(this);
	toolBar->setMovable(false);
	// Menu
	auto menu = new QToolButton(this);
	menu->setText("Menu");
	menu->setIcon(QIcon::fromTheme("application-menu"));
	menu->setPopupMode(QToolButton::InstantPopup);
	menu->setMenu(createMenu());
	toolBar->addWidget(menu);
	toolBar->addSeparator();
	// Media controls
	auto previous = toolBar->addAction(QIcon::fromTheme("media-skip-backward"), "Previous");
	playPause = toolBar->addAction(QIcon::fromTheme("media-playback-start"), "Play");
	QAction::connect(playPause, &QAction::triggered, [=](bool checked) {
		auto status = playPause->iconText() == "Pause" ? spotify->pause() : spotify->resume();
		if (!status.isEmpty())
		{
			setStatus(QString("Failed to %1 playback: %2")
				.arg(playPause->iconText() == "Pause" ? "pause" : "resume")
				.arg(status));
		}
	});
	auto next = toolBar->addAction(QIcon::fromTheme("media-skip-forward"),  "Next");
	QAction::connect(previous, &QAction::triggered, [=](bool checked) {
		auto status = spotify->previous();
		if (!status.isEmpty())
			setStatus(QString("Failed to go to previous track: %1").arg(status));
		refresh();
	});
	QAction::connect(next, &QAction::triggered, [=](bool checked) {
		auto status = spotify->next();
		if (!status.isEmpty())
			setStatus(QString("Failed to go to next track: %1").arg(status));
		refresh();
	});
	// Progress
	progress = new QSlider(this);
	progress->setOrientation(Qt::Orientation::Horizontal);
	QSlider::connect(progress, &QAbstractSlider::sliderReleased, [=]() {
		auto status = spotify->seek(progress->value());
		if (!status.isEmpty())
			setStatus(QString("Failed to seek: %1").arg(status));
	});
	toolBar->addSeparator();
	toolBar->addWidget(progress);
	toolBar->addSeparator();
	position = new QLabel("0:00/0:00", this);
	toolBar->addWidget(position);
	toolBar->addSeparator();
	// Shuffle and repeat toggles
	shuffle = toolBar->addAction(QIcon::fromTheme("media-playlist-shuffle"), "Shuffle");
	shuffle->setCheckable(true);
	QAction::connect(shuffle, &QAction::triggered, [=](bool checked) {
		auto status = spotify->setShuffle(checked);
		if (!status.isEmpty())
			setStatus(QString("Failed to toggle shuffle: %1").arg(status));
		refresh();
	});
	repeat = toolBar->addAction(QIcon::fromTheme("media-playlist-repeat"), "Repeat");
	repeat->setCheckable(true);
	QAction::connect(repeat, &QAction::triggered, [=](bool checked) {
		auto status = spotify->setRepeat(checked ? "context" : "off");
		if (!status.isEmpty())
			setStatus(QString("Failed to toggle repeat: %1").arg(status));
		refresh();
	});
	// Volume slider
	volume = new QSlider(this);
	volume->setOrientation(Qt::Orientation::Horizontal);
	volume->setMaximumWidth(100);
	volume->setMinimum(0);
	volume->setMaximum(20);
	volume->setValue(20);
	toolBar->addWidget(volume);
	Settings settings;
	if (settings.pulseVolume())
	{
		// If using PulseAudio for volume control, update on every
		QSlider::connect(volume, &QAbstractSlider::valueChanged, [](int value) {
			QProcess process;
			// Find what sink to use
			process.start("/usr/bin/pactl", {
				"list", "sink-inputs"
			});
			process.waitForFinished();
			auto sinks = QString(process.readAllStandardOutput()).split("Sink Input #");
			QString sink;
			for (auto &s : sinks)
				if (s.contains("Spotify"))
					sink = s;
			if (sink.isEmpty())
				return;
			// Sink was found, get id
			auto left = sink.left(sink.indexOf('\n'));
			auto sinkId = left.right(left.length() - left.lastIndexOf('#') - 1);
			process.start("/usr/bin/pactl", {
				"set-sink-input-volume", sinkId, QString::number(value * 0.05, 'f', 2)
			});
			process.waitForFinished();
		});
	}
	else
	{
		// If using Spotify for volume control, only update on release
		QSlider::connect(volume, &QAbstractSlider::sliderReleased, [=]() {
			auto status = spotify->setVolume(volume->value() * 5);
			if (!status.isEmpty())
				setStatus(QString("Failed to set volume: %1").arg(status));
		});
	}
	// Return final tool bar
	return toolBar;
}

QAction *createMenuAction(const QString &icon, const QString &text, QKeySequence::StandardKey shortcut)
{
	auto action = new QAction(QIcon::fromTheme(icon), text);
	if (shortcut != QKeySequence::UnknownKey)
		action->setShortcut(QKeySequence(shortcut));
	return action;
}

void MainWindow::refreshDevices(QMenu *deviceMenu)
{
	// Set status and get devices
	setStatus("Refreshing devices...");
	auto devices = spotify->devices();
	// Clear all entries
	for (auto &action : deviceMenu->actions())
		deviceMenu->removeAction(action);
	// Check if empty
	if (devices.isEmpty())
	{
		setStatus("No devices found");
		deviceMenu->addAction("No devices found")->setDisabled(true);
		return;
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
			auto status = spotify->setDevice(device);
			if (!status.isEmpty())
			{
				action->setChecked(false);
				setStatus(QString("Failed to set device: %1").arg(status));
			}
			else
				action->setDisabled(true);
		});
	}
}

QTreeWidgetItem *treeItem(QTreeWidget *tree, const QString &key, const QString &value)
{
	return new QTreeWidgetItem(tree, {
		key, value
	});
}

void MainWindow::openAudioFeaturesWidget(const QString &trackId, const QString &artist, const QString &name)
{
	auto features = spotify->trackAudioFeatures(trackId);
	auto dock = new QDockWidget(QString("%1 - %2")
		.arg(artist)
		.arg(name)
		.replace("&", "&&"), this);
	auto tree = new QTreeWidget(dock);
	tree->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tree->header()->hide();
	tree->setSelectionMode(QAbstractItemView::NoSelection);
	tree->setRootIsDecorated(false);
	tree->setAllColumnsShowFocus(true);
	tree->setColumnCount(2);
	tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	QMapIterator<QString, QString> i(features.values);
	while (i.hasNext())
	{
		i.next();
		tree->addTopLevelItem(treeItem(tree, i.key(), i.value()));
	}
	dock->setWidget(tree);
	dock->setMinimumWidth(150);
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dock);
}

void MainWindow::openLyrics(const QString &artist, const QString &name)
{
	setStatus("Loading lyrics...");
	auto reply = network->get(QNetworkRequest(QUrl(QString(
		"https://vscode-spotify-lyrics.azurewebsites.net/lyrics?artist=%1&title=%2")
			.arg(artist)
			.arg(name))));
	while (!reply->isFinished())
		QCoreApplication::processEvents();
	auto lyricsText = QString(reply->readAll()).trimmed();
	if (lyricsText == "Not found")
	{
		setStatus("Lyrics not found");
		return;
	}
	setStatus("Lyrics loaded");
	auto dock = new QDockWidget(
		QString("%1 - %2")
			.arg(artist)
			.arg(name), this);
	auto lyricsView = new QTextEdit(dock);
	lyricsView->setHtml(lyricsText.replace("\n", "<br/>"));
	lyricsView->setReadOnly(true);
	dock->setWidget(lyricsView);
	dock->setMinimumWidth(300);
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dock);
}

QMenu *MainWindow::createMenu()
{
	// Create root
	auto menu = new QMenu(this);
	// About
	auto aboutMenu = new QMenu("About");
	auto aboutQt = createMenuAction("qt", "About Qt", QKeySequence::UnknownKey);
	QAction::connect(aboutQt, &QAction::triggered, [=](bool checked) {
		QMessageBox::aboutQt(this);
	});
	auto checkForUpdates = createMenuAction("download", "Check for updates", QKeySequence::UnknownKey);
	QAction::connect(checkForUpdates, &QAction::triggered, [=](bool checked) {
		setStatus("Checking for updates...");
		auto json = getJson("https://api.github.com/repos/kraxarn/spotify-qt/releases");
		auto latest = json.array()[0].toObject()["tag_name"].toString();
		setStatus(latest == APP_VERSION
				  ? "You have the latest version"
				  : QString("Update found, latest version is %1, you have version %2")
					  .arg(latest)
					  .arg(APP_VERSION));
	});
	aboutMenu->setIcon(QIcon::fromTheme("help-about"));
	aboutMenu->addAction(QString("spotify-qt %1").arg(APP_VERSION))->setDisabled(true);
	aboutMenu->addActions({
		aboutQt, checkForUpdates
	});
	menu->addMenu(aboutMenu);
	// Device selection
	auto deviceMenu = new QMenu("Device");
	deviceMenu->setIcon(QIcon::fromTheme("speaker"));
	QMenu::connect(deviceMenu, &QMenu::aboutToShow, [=]() {
		refreshDevices(deviceMenu);
	});
	menu->addMenu(deviceMenu);
	// Refresh and settings
	auto openSettings = createMenuAction("settings", "Settings...", QKeySequence::Preferences);
	QAction::connect(openSettings, &QAction::triggered, [=]() {
		SettingsDialog dialog(this);
		dialog.exec();
	});
	menu->addAction(openSettings);
	// Log out and quit
	menu->addSeparator();
	auto quitAction = createMenuAction("application-exit", "Quit",    QKeySequence::Quit);
	QAction::connect(quitAction, &QAction::triggered, QCoreApplication::quit);
	menu->addActions({
		createMenuAction("im-user-away",     "Log out", QKeySequence::UnknownKey),
		quitAction
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

bool MainWindow::loadSongs(const QVector<spt::Track> &tracks)
{
	songs->clear();
	for (int i = 0; i < tracks.length(); i++)
	{
		auto track = tracks.at(i);
		auto item = new QTreeWidgetItem({
			"", track.name, track.artist, track.album, formatTime(track.duration)
		});
		item->setData(0, RoleTrackId,  QString("spotify:track:%1").arg(track.id));
		item->setData(0, RoleArtistId, track.artistId);
		item->setData(0, RoleAlbumId,  track.albumId);
		if (track.isLocal)
		{
			item->setDisabled(true);
			item->setToolTip(1, "Local track");
		}
		if (current.item != nullptr && track.id == current.item->id)
			item->setIcon(0, QIcon::fromTheme("media-playback-start"));
		songs->insertTopLevelItem(i, item);
	}
	return true;
}

bool MainWindow::loadPlaylist(spt::Playlist &playlist)
{
	songs->setEnabled(false);
	auto result = loadSongs(playlist.loadTracks(*spotify));
	songs->setEnabled(true);
	sptContext = QString("spotify:playlist:%1").arg(playlist.id);
	return result;
}

void MainWindow::setStatus(const QString &message)
{
	statusBar()->showMessage(message, 5000);
}

void MainWindow::setCurrentSongIcon()
{
	for (int i = 0; i < songs->topLevelItemCount(); i++)
	{
		auto item = songs->topLevelItem(i);
		if (item->data(0, RoleTrackId).toString() == QString("spotify:track:%1").arg(current.item->id))
			item->setIcon(0, QIcon::fromTheme("media-playback-start"));
		else
			item->setIcon(0, QIcon());
	}
}

void MainWindow::setAlbumImage(const QString &url)
{
	auto reply = network->get(QNetworkRequest(QUrl(url)));
	while (!reply->isFinished())
		QCoreApplication::processEvents();
	QPixmap art;
	art.loadFromData(reply->readAll(), "jpeg");
	nowAlbum->setPixmap(art);
}

QString MainWindow::formatTime(int ms)
{
	auto duration = QTime(0, 0).addMSecs(ms);
	return QString("%1:%2")
		.arg(duration.minute())
		.arg(duration.second() % 60, 2, 10, QChar('0'));
}

QByteArray MainWindow::get(const QString &url)
{
	auto reply = network->get(QNetworkRequest(QUrl(url)));
	while (!reply->isFinished())
		QCoreApplication::processEvents();
	reply->deleteLater();
	return reply->readAll();
}

QJsonDocument MainWindow::getJson(const QString &url)
{
	return QJsonDocument::fromJson(get(url));
}

void MainWindow::openArtist(const QString &artistId)
{
	auto artist = spotify->artist(artistId);
	auto dock = new QDockWidget(artist.name, this);
	auto layout = new QVBoxLayout();
	//layout->setContentsMargins(0, 0, 0, 0);
	// Get cover image (320x320 -> 320x160)
	QPixmap cover;
	cover.loadFromData(get(artist.image), "jpeg");
	auto coverLabel = new QLabel(dock);
	coverLabel->setPixmap(cover.copy(0, 80, 320, 160));
	layout->addWidget(coverLabel);
	// Artist name title
	auto title = new QLabel(artist.name, dock);
	title->setAlignment(Qt::AlignHCenter);
	auto titleFont = title->font();
	titleFont.setPointSize(24);
	title->setFont(titleFont);
	layout->addWidget(title);
	// Genres
	auto genres = new QLabel(artist.genres.join(", "));
	genres->setWordWrap(true);
	layout->addWidget(genres);
	// Tabs
	auto tabs = new QTabWidget(dock);
	layout->addWidget(tabs);
	// Top tracks
	auto topTracks = artist.topTracks(*spotify);
	QStringList topTrackIds;
	auto topTracksList = new QListWidget(tabs);
	for (auto &track : topTracks)
	{
		auto item = new QListWidgetItem(track.name, topTracksList);
		QPixmap iconData;
		iconData.loadFromData(get(track.image), "jpeg");
		item->setIcon(QIcon(iconData));
		item->setData(RoleTrackId, track.id);
		topTrackIds.append(QString("spotify:track:%1").arg(track.id));
	}
	QListWidget::connect(topTracksList, &QListWidget::itemClicked, [this, topTrackIds](QListWidgetItem *item) {
		auto result =  spotify->playTracks(
			QString("spotify:track:%1").arg(item->data(RoleTrackId).toString()), topTrackIds);
		if (!result.isEmpty())
			setStatus(QString("Failed to start playback: %1").arg(result));
	});
	tabs->addTab(topTracksList, "Popular");
	tabs->addTab(new QWidget(tabs), "Albums");
	tabs->addTab(new QWidget(tabs), "Appears on");
	tabs->addTab(new QWidget(tabs), "Related");

	dock->setWidget(layoutToWidget(layout));
	dock->setFixedWidth(320);
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, dock);
}