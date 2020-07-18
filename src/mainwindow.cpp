#include "mainwindow.hpp"

bool MainWindow::darkBackground	= false;

MainWindow::MainWindow(Settings &settings)
	: settings(settings), QMainWindow()
{
	// Set cache root location
	cacheLocation = QStandardPaths::standardLocations(QStandardPaths::CacheLocation)[0];

	// Create main cache path and album subdir
	QDir cacheDir(cacheLocation);
	cacheDir.mkpath(".");
	cacheDir.mkdir("album");
	cacheDir.mkdir("playlist");
	cacheDir.mkdir("tracks");

	// Apply selected style and palette
	QApplication::setStyle(settings.general.style);
	applyPalette(settings.general.stylePalette);

	// Check for dark background
	auto bg = palette().color(backgroundRole());
	if (((bg.red() + bg.green() + bg.blue()) / 3) < 128)
		darkBackground = true;

	// Set Spotify
	spotify = new spt::Spotify(settings, this);
	network = new QNetworkAccessManager();

	// Setup main window
	setWindowTitle("spotify-qt");
	setWindowIcon(Icon::get("logo:spotify-qt"));
	resize(1280, 720);
	setCentralWidget(createCentralWidget());
	addToolBar(Qt::ToolBarArea::TopToolBarArea, createToolBar());

	// Update player status
	auto timer = new QTimer(this);
	QTimer::connect(timer, &QTimer::timeout, this, &MainWindow::refresh);
	refresh();
	timer->start(1000);

	// Check if should start client
	if (settings.spotify.startClient
		&& (settings.spotify.alwaysStart || spotify->devices().isEmpty()))
	{
		sptClient = new spt::ClientHandler(settings, this);
		auto status = sptClient->start();
		if (!status.isEmpty())
			QMessageBox::warning(
				this,
				"Client error",
				QString("Failed to autostart Spotify client: %1").arg(status));
	}

	// Start media controller if specified
	if (settings.general.mediaController)
	{
		mediaPlayer = new mp::Service(spotify, this);
		// Check if something went wrong during init
		if (!mediaPlayer->isValid())
		{
			delete mediaPlayer;
			mediaPlayer = nullptr;
		}
	}

	// Start listening to current playback responses
	spt::Spotify::connect(spotify, &spt::Spotify::gotPlayback, [this](const spt::Playback &playback)
	{
		refreshed(playback);
	});

	// Create tray icon if specified
	if (settings.general.trayIcon)
		trayIcon = new TrayIcon(spotify, settings, this);

	// If new version has been detected, show what's new dialog
	if (settings.general.showChangelog && settings.general.lastVersion != APP_VERSION)
		(new WhatsNewDialog(APP_VERSION, settings, this))->open();
	settings.general.lastVersion = APP_VERSION;
	settings.save();

	// Welcome
	setStatus("Welcome to spotify-qt!");
}

void MainWindow::refresh()
{
	if (refreshCount < 0
		|| ++refreshCount >= settings.general.refreshInterval
		|| current.progressMs + 1000 > current.item.duration)
	{
		spotify->requestCurrentPlayback();
		refreshCount = 0;
		return;
	}
	// Assume last refresh was 1 sec ago
	if (!current.isPlaying)
		return;
	current.progressMs += 1000;
	refreshed(current);
}

void MainWindow::refreshed(const spt::Playback &playback)
{
	// Just so it doesn't make the app startup slower for now
	if (currentUser.id.isEmpty())
		currentUser = spotify->me();
	current = playback;
	if (!current.isPlaying && current.item.name == "(no name)")
	{
		playPause->setIcon(Icon::get("media-playback-start"));
		playPause->setText("Play");
		return;
	}
	auto currPlaying = QString("%1\n%2").arg(current.item.name).arg(current.item.artist);
	if (nowPlaying->text() != currPlaying)
	{
		if (current.isPlaying)
			setPlayingTrackItem(trackItems.contains(current.item.id)
				? trackItems[current.item.id] : nullptr);
		nowPlaying->setText(currPlaying);
		setAlbumImage(current.item.image);
		setWindowTitle(QString("%1 - %2").arg(current.item.artist).arg(current.item.name));
		if (mediaPlayer != nullptr)
			mediaPlayer->currentSourceChanged(current);
		if (trayIcon != nullptr && settings.general.trayAlbumArt)
			trayIcon->setPixmap(getAlbum(current.item.image));
	}
	position->setText(QString("%1/%2")
		.arg(formatTime(current.progressMs))
		.arg(formatTime(current.item.duration)));
	progress->setValue(current.progressMs);
	progress->setMaximum(current.item.duration);
	playPause->setIcon(Icon::get(
		current.isPlaying ? "media-playback-pause" : "media-playback-start"));
	playPause->setText(current.isPlaying ? "Pause" : "Play");
	if (!settings.general.pulseVolume)
		volumeButton->setVolume(current.volume / 5);
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

QWidget *MainWindow::layoutToWidget(QLayout *layout)
{
	auto widget = new QWidget();
	widget->setLayout(layout);
	return widget;
}

QTreeWidgetItem *MainWindow::treeItem(QTreeWidget *tree, const QString &name, const QString &toolTip, const QStringList &childrenItems)
{
	auto item = new QTreeWidgetItem(tree, {name});
	item->setToolTip(0, toolTip);
	for (auto &child : childrenItems)
		item->addChild(new QTreeWidgetItem(item, {child}));
	return item;
}

QWidget *MainWindow::createCentralWidget()
{
	auto container = new QSplitter();
	// Sidebar with playlists etc.
	auto sidebar = new QVBoxLayout();
	libraryList = new QTreeWidget(this);
	playlists = new QListWidget(this);
	// Library
	libraryList->addTopLevelItems({
		treeItem(libraryList, "Recently Played", "Most recently played tracks from any device", QStringList()),
		treeItem(libraryList, "Liked", "Liked and saved tracks", QStringList()),
		treeItem(libraryList, "Tracks", "Most played tracks for the past 6 months", QStringList()),
		treeItem(libraryList, "New Releases", "New albums from artists you listen to", QStringList()),
		treeItem(libraryList, "Albums", "Liked and saved albums"),
		treeItem(libraryList, "Artists", "Most played artists for the past 6 months")
	});
	libraryList->header()->hide();
	libraryList->setCurrentItem(nullptr);
	QTreeWidget::connect(libraryList, &QTreeWidget::itemClicked, [this](QTreeWidgetItem *item, int column) {
		if (item != nullptr) {
			playlists->setCurrentRow(-1);
			if (item->parent() != nullptr)
			{
				auto data = item->data(0, 0x100).toString();
				switch (item->data(0, 0x101).toInt())
				{
					case RoleArtistId:
						openArtist(data);
						break;

					case RoleAlbumId:
						loadAlbum(data, false);
						break;
				}
			}
			else
			{
				auto id = item->text(0).toLower().replace(' ', '_');
				auto cacheTracks = loadTracksFromCache(id);
				if (cacheTracks.isEmpty())
					songs->setEnabled(false);
				else
					loadSongs(cacheTracks);

				QVector<spt::Track> tracks;
				if (item->text(0) == "Recently Played")
					tracks = spotify->recentlyPlayed();
				else if (item->text(0) == "Liked")
					tracks = spotify->savedTracks();
				else if (item->text(0) == "Tracks")
					tracks = spotify->topTracks();
				else if (item->text(0) == "New Releases")
				{
					auto all = allArtists();
					auto releases = spotify->newReleases();
					for (auto &album : releases)
						if (all.contains(album.artist))
							for (auto &track : spotify->albumTracks(album.id))
							{
								track.addedAt = album.releaseDate;
								tracks << track;
							}
				}

				saveTracksToCache(id, tracks);
				loadSongs(tracks);
				songs->setEnabled(true);
			}
		}
	});
	QTreeWidget::connect(libraryList, &QTreeWidget::itemDoubleClicked, [this](QTreeWidgetItem *item, int column) {
		// Fetch all tracks in list
		auto tracks = item->text(0) == "Recently Played"
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
			setStatus(QString("Failed to start playback: %1").arg(status), true);
	});
	// When expanding top artists, update it
	QTreeWidget::connect(libraryList, &QTreeWidget::itemExpanded, [this](QTreeWidgetItem *item) {
		QVector<QVariantList> results;
		item->takeChildren();

		if (item->text(0) == "Artists")
			for (auto &artist : spotify->topArtists())
				results.append({artist.name, artist.id, RoleArtistId});
		else if (item->text(0) == "Albums")
			for (auto &album : spotify->savedAlbums())
				results.append({album.name, album.id, RoleAlbumId});

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
	auto library = createGroupBox(QVector<QWidget*>() << libraryList);
	library->setTitle("Library");
	sidebar->addWidget(library);
	// Update current playlists
	refreshPlaylists();
	// Set default selected playlist
	playlists->setCurrentRow(0);
	QListWidget::connect(playlists, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
		if (item != nullptr)
			libraryList->setCurrentItem(nullptr);
		auto currentPlaylist = sptPlaylists.at(playlists->currentRow());
		loadPlaylist(currentPlaylist);
	});
	QListWidget::connect(playlists, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
		auto currentPlaylist = sptPlaylists.at(playlists->currentRow());
		loadPlaylist(currentPlaylist);
		auto result = spotify->playTracks(
			QString("spotify:playlist:%1").arg(currentPlaylist.id));
		if (!result.isEmpty())
			setStatus(QString("Failed to start playlist playback: %1").arg(result), true);
	});
	playlists->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(playlists, &QWidget::customContextMenuRequested, [=](const QPoint &pos) {
		(new PlaylistMenu(*spotify, sptPlaylists.at(playlists->currentRow()), this))
			->popup(playlists->mapToGlobal(pos));
	});
	auto playlistContainer = createGroupBox(QVector<QWidget*>() << playlists);
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
	QLabel::connect(nowPlaying, &QWidget::customContextMenuRequested, [this](const QPoint &pos) {
		auto track = current.item;
		songMenu(track.id, track.artist, track.name, track.artistId, track.albumId)
			->popup(nowPlaying->mapToGlobal(pos));
	});
	// Sidebar as widget
	auto sidebarWidget = layoutToWidget(sidebar);
	sidebarWidget->setMaximumWidth(250);
	container->addWidget(sidebarWidget);
	// Table with songs
	songs = new QTreeWidget(this);
	songs->setEditTriggers(QAbstractItemView::NoEditTriggers);
	songs->setSelectionBehavior(QAbstractItemView::SelectRows);
	songs->setSortingEnabled(true);
	songs->setRootIsDecorated(false);
	songs->setAllColumnsShowFocus(true);
	songs->setColumnCount(5);
	songs->setHeaderLabels({
		" ", "Title", "Artist", "Album", "Length", "Added"
	});
	songs->header()->setSectionsMovable(false);
	songs->header()->setSectionResizeMode((QHeaderView::ResizeMode) settings.general.songHeaderResizeMode);
	if (settings.general.songHeaderSortBy > 0)
		songs->header()->setSortIndicator(settings.general.songHeaderSortBy + 1, Qt::AscendingOrder);
	// Hide specified columns
	for (auto &value : settings.general.hiddenSongHeaders)
		songs->header()->setSectionHidden(value + 1, true);
	// Song context menu
	songs->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(songs, &QWidget::customContextMenuRequested, [this](const QPoint &pos) {
		auto item = songs->itemAt(pos);
		auto trackId = item->data(0, RoleTrackId).toString();
		if (trackId.isEmpty())
			return;
		songMenu(trackId, item->text(2), item->text(1),
			item->data(0, RoleArtistId).toString(),
			item->data(0, RoleAlbumId).toString())->popup(songs->mapToGlobal(pos));
	});
	QTreeWidget::connect(songs, &QTreeWidget::itemClicked, this, [=](QTreeWidgetItem *item, int column) {
		auto trackId = item->data(0, RoleTrackId).toString();
		if (trackId.isEmpty())
		{
			setStatus("Failed to start playback: track not found", true);
			return;
		}
		// If we played from library, we don't have any context
		auto allTracks = currentTracks();
		auto status = (libraryList->currentItem() != nullptr || !settings.general.spotifyPlaybackOrder)
			&& allTracks.count() < 500
			? spotify->playTracks(trackId, allTracks)
			: spotify->playTracks(trackId, sptContext);

		if (!status.isEmpty())
			setStatus(QString("Failed to start playback: %1").arg(status), true);
		else
			setPlayingTrackItem(item);
		refresh();
	});

	// Songs header context menu
	songs->header()->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(songs->header(), &QWidget::customContextMenuRequested, [this](const QPoint &pos) {
		auto menu = new QMenu(songs->header());
		auto showHeaders = menu->addMenu(Icon::get("visibility"), "Columns to show");
		auto sortBy = menu->addMenu(Icon::get("view-sort-ascending"), "Default sorting");
		auto headerTitles = QStringList({
			"Title", "Artist", "Album", "Length", "Added"
		});
		auto headers = settings.general.hiddenSongHeaders;
		for (int i = 0; i < headerTitles.size(); i++)
		{
			auto showTitle = showHeaders->addAction(headerTitles.at(i));
			showTitle->setCheckable(true);
			showTitle->setChecked(std::find(headers.begin(), headers.end(), i) == headers.end());
			showTitle->setData(QVariant(i));

			auto sortTitle = sortBy->addAction(headerTitles.at(i));
			sortTitle->setCheckable(true);
			sortTitle->setChecked(i == settings.general.songHeaderSortBy);
			sortTitle->setData(QVariant(100 + i));
		}
		QMenu::connect(menu, &QMenu::triggered, [this](QAction *action) {
			int i = action->data().toInt();
			// Columns to show
			if (i < 100)
			{
				songs->header()->setSectionHidden(i + 1, !action->isChecked());
				if (action->isChecked())
					settings.general.hiddenSongHeaders.erase(std::remove(settings.general.hiddenSongHeaders.begin(),
						settings.general.hiddenSongHeaders.end(), i));
				else
					settings.general.hiddenSongHeaders.push_back(i);
				settings.save();
				return;
			}
			// Sort by
			i -= 100;
			if (settings.general.songHeaderSortBy != i)
				songs->header()->setSortIndicator(i + 1, Qt::AscendingOrder);
			settings.general.songHeaderSortBy = settings.general.songHeaderSortBy == i ? -1 : i;
			settings.save();
		});
		menu->popup(songs->header()->mapToGlobal(pos));
	});

	// Load tracks in playlist
	auto playlistId = settings.general.lastPlaylist;
	if (sptPlaylists.isEmpty())
	{
		// If no playlists were found
		// TODO: Load something from library here
	}
	else if (playlistId.isEmpty())
	{
		// Default to first in list
		playlistId = sptPlaylists.at(0).id;
	}
	else
	{
		// Find playlist in list
		int i = 0;
		for (auto &playlist : sptPlaylists)
		{
			if (playlist.id == playlistId)
			{
				playlists->setCurrentRow(i);
				loadPlaylist(playlist);
			}
			i++;
		}
	}
	// Add to main thing
	container->addWidget(songs);
	return container;
}

QMenu *MainWindow::songMenu(const QString &trackId, const QString &artist,
	const QString &name, const QString &artistId, const QString &albumId)
{
	return new SongMenu(trackId, artist, name, artistId, albumId, spotify, this);
}

QToolBar *MainWindow::createToolBar()
{
	auto toolBar = new QToolBar("Media controls", this);
	toolBar->setMovable(false);
	// Menu
	auto menu = new QToolButton(this);
	menu->setText("Menu");
	menu->setIcon(Icon::get("application-menu"));
	menu->setPopupMode(QToolButton::InstantPopup);
	menu->setMenu(new MainMenu(*spotify, settings, this));
	toolBar->addWidget(menu);
	// Search
	search = toolBar->addAction(Icon::get("edit-find"), "Search");
	search->setCheckable(true);
	searchView = new SearchView(*spotify, this);
	addDockWidget(Qt::RightDockWidgetArea, searchView);
	searchView->hide();
	QAction::connect(search, &QAction::triggered, [this](bool checked) {
		searchView->setHidden(!checked);
	});
	// Media controls
	toolBar->addSeparator();
	auto previous = toolBar->addAction(Icon::get("media-skip-backward"), "Previous");
	playPause = toolBar->addAction(Icon::get("media-playback-start"), "Play");
	QAction::connect(playPause, &QAction::triggered, [=](bool checked) {
		current.isPlaying = !current.isPlaying;
		refreshed(current);
		auto status = playPause->iconText() == "Play" ? spotify->pause() : spotify->resume();
		if (!status.isEmpty())
		{
			setStatus(QString("Failed to %1 playback: %2")
				.arg(playPause->iconText() == "Pause" ? "pause" : "resume")
				.arg(status), true);
		}
	});
	auto next = toolBar->addAction(Icon::get("media-skip-forward"),  "Next");
	QAction::connect(previous, &QAction::triggered, [=](bool checked) {
		auto status = spotify->previous();
		if (!status.isEmpty())
			setStatus(QString("Failed to go to previous track: %1").arg(status), true);
		refresh();
	});
	QAction::connect(next, &QAction::triggered, [=](bool checked) {
		auto status = spotify->next();
		if (!status.isEmpty())
			setStatus(QString("Failed to go to next track: %1").arg(status), true);
		refresh();
	});
	// Progress
	progress = new QSlider(this);
	progress->setOrientation(Qt::Orientation::Horizontal);
	QSlider::connect(progress, &QAbstractSlider::sliderReleased, [=]() {
		auto status = spotify->seek(progress->value());
		if (!status.isEmpty())
			setStatus(QString("Failed to seek: %1").arg(status), true);
		if (mediaPlayer != nullptr)
			mediaPlayer->stateUpdated();
	});
	toolBar->addSeparator();
	toolBar->addWidget(progress);
	toolBar->addSeparator();
	position = new QLabel("0:00/0:00", this);
	if (settings.general.fixedWidthTime)
		position->setFont(QFont("monospace"));
	toolBar->addWidget(position);
	toolBar->addSeparator();
	// Shuffle and repeat toggles
	shuffle = toolBar->addAction(Icon::get("media-playlist-shuffle"), "Shuffle");
	shuffle->setCheckable(true);
	QAction::connect(shuffle, &QAction::triggered, [=](bool checked) {
		current.shuffle = !current.shuffle;
		refreshed(current);
		auto status = spotify->setShuffle(checked);
		if (!status.isEmpty())
			setStatus(QString("Failed to toggle shuffle: %1").arg(status), true);
	});
	repeat = toolBar->addAction(Icon::get("media-playlist-repeat"), "Repeat");
	repeat->setCheckable(true);
	QAction::connect(repeat, &QAction::triggered, [=](bool checked) {
		auto repeatMode = QString(checked ? "context" : "off");
		current.repeat = repeatMode;
		refreshed(current);
		auto status = spotify->setRepeat(repeatMode);
		if (!status.isEmpty())
			setStatus(QString("Failed to toggle repeat: %1").arg(status), true);
	});
	// Volume
	volumeButton = new VolumeButton(settings, *spotify, this);
	toolBar->addWidget(volumeButton);
	// Return final tool bar
	return toolBar;
}

void MainWindow::openAudioFeaturesWidget(const QString &trackId, const QString &artist, const QString &name)
{
	auto view = new AudioFeaturesView(*spotify, trackId, artist, name, this);
	if (audioFeaturesView != nullptr)
	{
		audioFeaturesView->close();
		audioFeaturesView->deleteLater();
	}
	audioFeaturesView = view;
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, audioFeaturesView);
}

void MainWindow::openLyrics(const QString &artist, const QString &name)
{
	auto view = new LyricsView(artist, name, this);
	if (!view->lyricsFound())
	{
		view->deleteLater();
		return;
	}
	if (lyricsView != nullptr)
	{
		lyricsView->close();
		lyricsView->deleteLater();
	}
	lyricsView = view;
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, lyricsView);
}

void MainWindow::refreshPlaylists()
{
	auto lastIndex =
		playlists == nullptr
		? -1
		: playlists->currentRow();
	sptPlaylists = spotify->playlists();

	// Add all playlists
	playlists->clear();
	for (auto &playlist : sptPlaylists)
	{
		auto item = new QListWidgetItem(playlist.name, playlists);
		item->setToolTip(playlist.description);
		item->setData(RolePlaylistId, playlist.id);
	}
	if (lastIndex >= 0)
		playlists->setCurrentRow(lastIndex);
}

bool MainWindow::loadSongs(const QVector<spt::Track> &tracks)
{
	songs->clear();
	trackItems.clear();
	playingTrackItem = nullptr;
	for (int i = 0; i < tracks.length(); i++)
	{
		auto track = tracks.at(i);
		auto item = new QTreeWidgetItem({
			"", track.name, track.artist, track.album,
			formatTime(track.duration), track.addedAt.date().toString(Qt::SystemLocaleShortDate)
		});
		item->setData(0, RoleTrackId,  QString("spotify:track:%1").arg(track.id));
		item->setData(0, RoleArtistId, track.artistId);
		item->setData(0, RoleAlbumId,  track.albumId);
		item->setData(0, RoleIndex,    i);
		if (track.isLocal)
		{
			item->setDisabled(true);
			item->setToolTip(1, "Local track");
		}
		else if (track.id == current.item.id)
			setPlayingTrackItem(item);
		songs->insertTopLevelItem(i, item);
		trackItems[track.id] = item;
	}
	return true;
}

bool MainWindow::loadAlbum(const QString &albumId, bool ignoreEmpty)
{
	auto tracks = loadTracksFromCache(albumId);
	if (tracks.isEmpty())
		tracks = spotify->albumTracks(albumId);

	if (ignoreEmpty && tracks.length() <= 1)
		setStatus("Album only contains one song or is empty", true);
	else
	{
		playlists->setCurrentRow(-1);
		libraryList->setCurrentItem(nullptr);
		sptContext = QString("spotify:album:%1").arg(albumId);
		loadSongs(tracks);
		saveTracksToCache(albumId, tracks);
	}

	return true;
}

QVector<spt::Track> MainWindow::loadTracksFromCache(const QString &id)
{
	QVector<spt::Track> tracks;
	auto filePath = QString("%1/tracks/%2.json").arg(cacheLocation).arg(id);
	if (!QFileInfo::exists(filePath))
		return tracks;
	QFile file(filePath);
	file.open(QIODevice::ReadOnly);
	auto json = QJsonDocument::fromJson(file.readAll());
	file.close();
	if (json.isNull())
		return tracks;
	for (auto track : json.array())
		tracks.append(spt::Track(track.toObject()));
	return tracks;
}

void MainWindow::saveTracksToCache(const QString &id, const QVector<spt::Track> &tracks)
{
	QJsonArray json;
	for (auto &track : tracks)
		json.append(track.toJson());
	QFile file(QString("%1/tracks/%2.json").arg(cacheLocation).arg(id));
	file.open(QIODevice::WriteOnly);
	file.write(QJsonDocument(json).toJson());
}

bool MainWindow::loadPlaylist(spt::Playlist &playlist)
{
	settings.general.lastPlaylist = playlist.id;
	settings.save();
	if (loadPlaylistFromCache(playlist))
		return true;
	songs->setEnabled(false);
	auto result = loadSongs(playlist.loadTracks(*spotify));
	songs->setEnabled(true);
	sptContext = QString("spotify:playlist:%1").arg(playlist.id);
	if (result)
		cachePlaylist(playlist);
	return result;
}

bool MainWindow::loadPlaylistFromCache(spt::Playlist &playlist)
{
	auto tracks = playlistTracks(playlist.id);
	if (tracks.isEmpty())
		return false;
	songs->setEnabled(false);
	auto result = loadSongs(tracks);
	songs->setEnabled(true);
	sptContext = QString("spotify:playlist:%1").arg(playlist.id);
	refreshPlaylist(playlist);
	return result;
}

QVector<spt::Track> MainWindow::playlistTracks(const QString &playlistId)
{
	QVector<spt::Track> tracks;
	auto filePath = QString("%1/playlist/%2.json").arg(cacheLocation).arg(playlistId);
	if (!QFileInfo::exists(filePath))
		return tracks;
	QFile file(filePath);
	file.open(QIODevice::ReadOnly);
	auto json = QJsonDocument::fromJson(file.readAll());
	file.close();
	if (json.isNull())
		return tracks;
	for (auto track : json.object()["tracks"].toArray())
		tracks.append(spt::Track(track.toObject()));
	return tracks;
}

void MainWindow::refreshPlaylist(spt::Playlist &playlist)
{
	auto newPlaylist = spotify->playlist(playlist.id);
	auto tracks = newPlaylist.loadTracks(*spotify);
	if (sptContext.endsWith(playlist.id))
		loadSongs(tracks);
	cachePlaylist(newPlaylist);
}

void MainWindow::setStatus(const QString &message, bool important)
{
	if (trayIcon != nullptr && settings.general.trayNotifications)
	{
		if (important)
			trayIcon->message(message);
	}
	else
		statusBar()->showMessage(message, 5000);
}

void MainWindow::setAlbumImage(const QString &url)
{
	nowAlbum->setPixmap(mask(getAlbum(url)));
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

QPixmap MainWindow::getImage(const QString &type, const QString &url)
{
	QPixmap img;
	if (url.isEmpty())
		return img;
	// Check if cache exists
	auto cachePath = QString("%1/%2/%3").arg(cacheLocation).arg(type).arg(QFileInfo(url).baseName());
	if (QFileInfo::exists(cachePath))
	{
		// Read file from cache
		QFile file(cachePath);
		file.open(QIODevice::ReadOnly);
		img.loadFromData(file.readAll(), "jpeg");
	}
	else
	{
		// Download image and save to cache
		img.loadFromData(get(url), "jpeg");
		if (!img.save(cachePath, "jpeg"))
			qDebug() << "failed to save album cache to" << cachePath;
	}
	return img;
}

QPixmap MainWindow::getAlbum(const QString &url)
{
	return getImage("album", url);
}

void MainWindow::openArtist(const QString &artistId)
{
	auto view = new ArtistView(spotify, artistId, this);
	if (artistView != nullptr)
	{
		artistView->close();
		artistView->deleteLater();
	}
	artistView = view;
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea, artistView);
	libraryList->setCurrentItem(nullptr);
}

void MainWindow::cachePlaylist(spt::Playlist &playlist)
{
	// Remove old format if needed before caching
	auto baseFile = QString("%1/playlist/%2").arg(cacheLocation).arg(playlist.id);
	if (QFileInfo::exists(baseFile))
		QFile(baseFile).remove();
	// Save new
	QJsonDocument json(playlist.toJson(*spotify));
	QFile file(QString("%1.json").arg(baseFile));
	file.open(QIODevice::WriteOnly);
	file.write(json.toJson());
}

void MainWindow::applyPalette(Settings::Palette palette)
{
	QPalette p;
	switch (palette)
	{
		case Settings::paletteApp:		p = QApplication::palette(); 					break;
		case Settings::paletteStyle:	p = QApplication::style()->standardPalette();	break;
		case Settings::paletteDark:		p = DarkPalette();								break;
	}
	QApplication::setPalette(p);
}

QStringList MainWindow::currentTracks()
{
	QStringList tracks;
	tracks.reserve(songs->topLevelItemCount());
	for (int i = 0; i < songs->topLevelItemCount(); i++)
	{
		auto trackId = songs->topLevelItem(i)->data(0, RoleTrackId).toString();
		// spotify:track: = 14
		if (trackId.length() > 14)
			tracks.append(trackId);
	}
	return tracks;
}

spt::Playback MainWindow::currentPlayback()
{
	return current;
}

bool MainWindow::hasPlaylistSelected()
{
	return playlists->currentRow() >= 0;
}

QString MainWindow::currentLibraryItem()
{
	return libraryList->currentIndex().row() >= 0
		? libraryList->currentItem()->text(0)
		: QString();
}

void MainWindow::reloadTrayIcon()
{
	if (trayIcon != nullptr)
	{
		delete trayIcon;
		trayIcon = nullptr;
	}
	if (settings.general.trayIcon)
		trayIcon = new TrayIcon(spotify, settings, this);
}

bool MainWindow::hasDarkBackground()
{
	return darkBackground;
}

void MainWindow::setPlayingTrackItem(QTreeWidgetItem *item)
{
	if (playingTrackItem != nullptr)
		playingTrackItem->setIcon(0, QIcon());
	if (item == nullptr)
	{
		playingTrackItem = nullptr;
		return;
	}
	item->setIcon(0, Icon::get("media-playback-start"));
	playingTrackItem = item;
}

spt::User MainWindow::getCurrentUser()
{
	return currentUser;
}

QSet<QString> MainWindow::allArtists()
{
	QSet<QString> artists;
	for (auto i = 0; i < playlists->count(); i++)
		for (auto &track : playlistTracks(playlists->item(i)->data(RolePlaylistId).toString()))
			artists << track.artist;
	return artists;
}

void MainWindow::setFixedWidthTime(bool value)
{
	position->setFont(value ? QFont("monospace") : QFont());
}

QPixmap MainWindow::mask(const QPixmap &source, MaskShape shape, const QVariant &data)
{
	if (source.isNull())
		return source;

	auto img = source.toImage().convertToFormat(QImage::Format_ARGB32);
	QImage out(img.size(), QImage::Format_ARGB32);
	out.fill(Qt::GlobalColor::transparent);
	QPainter painter(&out);
	painter.setOpacity(0);
	painter.setBrush(Qt::white);
	painter.setPen(Qt::NoPen);
	painter.drawImage(0, 0, img);
	painter.setOpacity(1);
	QPainterPath path(QPointF(0, 0));

	QPolygonF polygon;
	switch (shape)
	{
		case MaskShape::App:

			polygon << QPointF(img.width() / 4, 0)
					<< QPointF(img.width(), 0)
					<< QPointF(img.width(), (img.height() / 4) * 3)
					<< QPointF((img.width() / 4) * 3, img.height())
					<< QPointF(0, img.height())
					<< QPointF(0, img.height() / 4);
			break;

		case MaskShape::Pie:
			switch (data.toInt() / 25)
			{
				case 0:
					polygon = QPolygonF(QRectF(
						img.width() / 2, 0,
						img.width() / 2, img.height() / 2));
					break;

				case 1:
					polygon = QPolygonF(QRectF(
						img.width() / 2, 0,
						img.width() / 2, img.height()));
					break;

				case 2:
					polygon << QPointF(img.width() / 2, 0)
							<< QPointF(img.width() / 2, img.height() / 2)
							<< QPointF(0, img.height() / 2)
							<< QPointF(0, img.height())
							<< QPointF(img.width(), img.height())
							<< QPointF(img.width(), 0);
					break;

				case 3:
					polygon = QPolygonF(QRectF(
						0, 0, img.width(), img.height()));
					break;
			}
			break;
	}

	path.addPolygon(polygon);
	painter.setClipPath(path);
	painter.drawImage(0, 0, img);
	return QPixmap::fromImage(out);
}

//region Getters

QString &MainWindow::getCacheLocation()
{
	return cacheLocation;
}

QVector<spt::Playlist> &MainWindow::getSptPlaylists()
{
	return sptPlaylists;
}

QListWidget *MainWindow::getPlaylistsList()
{
	return playlists;
}

QAction *MainWindow::getSearchAction()
{
	return search;
}

QTreeWidget *MainWindow::getSongsTree()
{
	return songs;
}

QString &MainWindow::getSptContext()
{
	return sptContext;
}

//endregion