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
	isPlaying	= false;
	mediaPlayer	= nullptr;
	// Set cache root location
	cacheLocation = QStandardPaths::standardLocations(QStandardPaths::CacheLocation)[0];
	// Create main cache path and album subdir
	QDir cacheDir(cacheLocation);
	cacheDir.mkpath(".");
	cacheDir.mkdir("album");
	cacheDir.mkdir("playlist");
	// Set Spotify
	spotify = new spt::Spotify();
	sptPlaylists = new QVector<spt::Playlist>();
	network = new QNetworkAccessManager();
	// Setup main window
	setWindowTitle("spotify-qt");
	setWindowIcon(Icon::get("logo:spotify-qt"));
	resize(1280, 720);
	setCentralWidget(createCentralWidget());
	addToolBar(Qt::ToolBarArea::TopToolBarArea, createToolBar());
	// Apply selected style and palette
	Settings settings;
	QApplication::setStyle(settings.style());
	applyPalette(settings.stylePalette());
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
	mediaPlayer = new mp::Service(spotify, this);
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
	delete	sptClient;
}

void MainWindow::refresh()
{
	current = spotify->currentPlayback();
	isPlaying = current.isPlaying;
	if (!current.isPlaying)
	{
		playPause->setIcon(Icon::get("media-playback-start"));
		playPause->setText("Play");
		return;
	}
	auto currPlaying = QString("%1\n%2").arg(current.item.name).arg(current.item.artist);
	if (nowPlaying->text() != currPlaying)
	{
		if (nowPlaying->text() != "No music playing")
			setCurrentSongIcon();
		nowPlaying->setText(currPlaying);
		setAlbumImage(current.item.image);
		setWindowTitle(QString("%1 - %2").arg(current.item.artist).arg(current.item.name));
		if (mediaPlayer != nullptr)
			mediaPlayer->currentSourceChanged(current);
	}
	position->setText(QString("%1/%2")
		.arg(formatTime(current.progressMs))
		.arg(formatTime(current.item.duration)));
	progress->setValue(current.progressMs);
	progress->setMaximum(current.item.duration);
	playPause->setIcon(Icon::get(
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
	auto libraryList = new QTreeWidget(this);
	playlists = new QListWidget();
	// Library
	libraryList->addTopLevelItems({
		treeItem(libraryList, "Recently Played", "Most recently played tracks from any device", QStringList()),
		treeItem(libraryList, "Liked", "Liked and saved tracks", QStringList()),
		treeItem(libraryList, "Tracks", "Most played tracks for the past 6 months", QStringList()),
		treeItem(libraryList, "Albums", "Liked and saved albums"),
		treeItem(libraryList, "Artists", "Most played artists for the past 6 months")
	});
	libraryList->header()->hide();
	QTreeWidget::connect(libraryList, &QTreeWidget::itemClicked, [this](QTreeWidgetItem *item, int column) {
		if (item != nullptr) {
			playlists->setCurrentRow(-1);
			if (item->parent() != nullptr)
			{
				auto data = item->data(0, 0x100).toString();
				switch (item->data(0, 0x101).toInt())
				{
					case RoleArtistId:	openArtist(data);		break;
					case RoleAlbumId:	loadAlbum(data, false);	break;
					case RoleTrackId:
						// Get all children of parent to get track ids
						QStringList trackIds;
						for (int i = 0; i < item->parent()->childCount(); i++)
							trackIds.append(QString("spotify:track:%1")
								.arg(item->parent()->child(i)->data(0, 0x100).toString()));
						// Play in context of all children
						spotify->playTracks(
							QString("spotify:track:%1").arg(item->data(0, 0x100).toString()), trackIds);
						break;
				}
			}
			else
			{
				if (item->text(0) == "Recently Played")
					loadSongs(spotify->recentlyPlayed());
				else if (item->text(0) == "Liked")
					loadSongs(spotify->savedTracks());
				else if (item->text(0) == "Tracks")
					loadSongs(spotify->topTracks());
			}
		}
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
	QListWidget::connect(playlists, &QListWidget::itemClicked, this, [this, libraryList](QListWidgetItem *item) {
		//if (item != nullptr)
		//	libraryList->setCurrentRow(-1);
		auto currentPlaylist = sptPlaylists->at(playlists->currentRow());
		loadPlaylist(currentPlaylist);
	});
	QListWidget::connect(playlists, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem *item) {
		auto currentPlaylist = sptPlaylists->at(playlists->currentRow());
		loadPlaylist(currentPlaylist);
		auto result = spotify->playTracks(
			QString("spotify:playlist:%1").arg(currentPlaylist.id));
		if (!result.isEmpty())
			setStatus(QString("Failed to start playlist playback: %1").arg(result));
	});
	auto playlistContainer = createGroupBox(QVector<QWidget*>() << playlists);
	playlistContainer->setTitle("Playlists");
	sidebar->addWidget(playlistContainer);
	// Now playing song
	auto nowPlayingLayout = new QHBoxLayout();
	nowPlayingLayout->setSpacing(12);
	nowAlbum = new QLabel();
	nowAlbum->setFixedSize(64, 64);
	nowAlbum->setPixmap(Icon::get("media-optical-audio").pixmap(nowAlbum->size()));
	nowPlayingLayout->addWidget(nowAlbum);
	nowPlaying = new QLabel("No music playing");
	nowPlaying->setWordWrap(true);
	nowPlayingLayout->addWidget(nowPlaying);
	sidebar->addLayout(nowPlayingLayout);
	// Show menu when clicking now playing
	nowPlaying->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(nowPlaying, &QWidget::customContextMenuRequested, [this](const QPoint &pos) {
		auto track = current.item;
		songMenu(nowPlaying, track.id, track.artist, track.name, track.artistId, track.albumId)
			->popup(nowPlaying->mapToGlobal(pos));
	});
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
		" ", "Title", "Artist", "Album", "Length", "Added"
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
		songMenu(songs, trackId, item->text(2), item->text(1),
			item->data(0, RoleArtistId).toString(),
			item->data(0, RoleAlbumId).toString())->popup(songs->mapToGlobal(pos));
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
	auto playlistId = Settings().lastPlaylist();
	// Default to first in list
	if (playlistId.isEmpty())
		playlistId = sptPlaylists->at(0).id;
	// Find playlist in list
	int i = 0;
	for (auto &playlist : *sptPlaylists)
	{
		if (playlist.id == playlistId)
		{
			playlists->setCurrentRow(i);
			loadPlaylist(playlist);
		}
		i++;
	}
	// Add to main thing
	container->addWidget(songs);
	return container;
}

QMenu *MainWindow::songMenu(QWidget *parent, const QString &trackId, const QString &artist,
	const QString &name, const QString &artistId, const QString &albumId)
{
	auto songMenu = new QMenu(parent);
	auto trackFeatures = songMenu->addAction(Icon::get("view-statistics"), "Audio features");
	QAction::connect(trackFeatures, &QAction::triggered, [=](bool checked) {
		openAudioFeaturesWidget(trackId, artist, name);
	});
	auto lyrics = songMenu->addAction(Icon::get("view-media-lyrics"), "Lyrics");
	QAction::connect(lyrics, &QAction::triggered, [=](bool checked) {
		openLyrics(artist, name);
	});
	auto share = songMenu->addMenu(Icon::get("document-share"), "Share");
	auto shareSongLink = share->addAction("Copy song link");
	QAction::connect(shareSongLink, &QAction::triggered, [=](bool checked) {
		QApplication::clipboard()->setText(
			QString("https://open.spotify.com/track/%1")
				.arg(QString(trackId).remove(0, QString("spotify:track:").length())));
		setStatus("Link copied to clipboard");
	});
	songMenu->addSeparator();
	// Add to playlist
	auto addPlaylist = songMenu->addMenu(Icon::get("list-add"), "Add to playlist");
	auto currentPlaylist = playlists->currentRow() == -1
		? nullptr : &sptPlaylists->at(playlists->currentRow());
	for (auto &playlist : *sptPlaylists)
	{
		// Create main action
		auto action = addPlaylist->addAction(playlist.name);
		action->setData(playlist.id);
	}
	QMenu::connect(addPlaylist, &QMenu::triggered, [this, trackId, currentPlaylist](QAction *action) {
		// Check if it's already in the playlist
		auto playlistId = action->data().toString();
		auto tracks = playlistTracks(playlistId);
		for (auto &track : tracks)
		{
			if (trackId.endsWith(track.id))
			{
				if (QMessageBox::information(this,
					 "Duplicate",
					 "Track is already in the playlist, do you want to add it anyway?",
					 QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)
					== QMessageBox::No)
					return;
				break;
			}
		}
		// Actually add
		auto result = spotify->addToPlaylist(playlistId, trackId);
		if (!result.isEmpty())
			setStatus(QString("Failed to add track to playlist: %1").arg(result));
	});
	// Remove from playlist
	auto remPlaylist = songMenu->addAction(Icon::get("list-remove"), "Remove from playlist");
	QAction::connect(remPlaylist, &QAction::triggered, [this, trackId, name, artist, currentPlaylist](bool checked) {
		// Remove from interface
		QTreeWidgetItem *item;
		int i;
		for (i = 0; i < songs->topLevelItemCount(); i++)
		{
			item = songs->topLevelItem(i);
			if (item->data(0, RoleTrackId).toString() == trackId)
				break;
			// Failed to find
			item = nullptr;
		}
		if (item == nullptr)
		{
			setStatus("Failed to remove track, not found in playlist");
			return;
		}
		// Remove from Spotify
		auto status = spotify->removeFromPlaylist(currentPlaylist->id, trackId,
			item->data(0, RoleIndex).toInt());
		// Update status
		if (!status.isEmpty())
		{
			setStatus(QString("Failed to remove track from playlist: %1").arg(status));
			return;
		}
		// i doesn't necessarily match item index depending on sorting order
		songs->takeTopLevelItem(i);
		setStatus(QString("Removed \"%1 - %2\" from \"%3\"")
			.arg(name).arg(artist).arg(currentPlaylist->name));
	});
	songMenu->addSeparator();
	auto goArtist = songMenu->addAction(Icon::get("view-media-artist"), "View artist");
	QAction::connect(goArtist, &QAction::triggered, [=](bool checked) {
		openArtist(artistId);
	});
	auto goAlbum = songMenu->addAction(Icon::get("view-media-album-cover"), "Open album");
	goAlbum->setEnabled(!sptContext.startsWith("spotify:album"));
	QAction::connect(goAlbum, &QAction::triggered, [=](bool checked) {
		loadAlbum(albumId);
	});
	return songMenu;
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
	menu->setMenu(new MainMenu(*spotify, this));
	toolBar->addWidget(menu);
	// Search
	search = toolBar->addAction(Icon::get("edit-find"), "Search");
	search->setCheckable(true);
	searchView = new SearchView(*spotify, this);
	addDockWidget(Qt::RightDockWidgetArea, searchView);
	searchView->hide();
	QAction::connect(search, &QAction::triggered, [this](bool checked) {
		if (checked)
			searchView->show();
		else
			searchView->hide();
	});
	// Media controls
	toolBar->addSeparator();
	auto previous = toolBar->addAction(Icon::get("media-skip-backward"), "Previous");
	playPause = toolBar->addAction(Icon::get("media-playback-start"), "Play");
	QAction::connect(playPause, &QAction::triggered, [=](bool checked) {
		auto status = playPause->iconText() == "Pause" ? spotify->pause() : spotify->resume();
		if (!status.isEmpty())
		{
			setStatus(QString("Failed to %1 playback: %2")
				.arg(playPause->iconText() == "Pause" ? "pause" : "resume")
				.arg(status));
		}
	});
	auto next = toolBar->addAction(Icon::get("media-skip-forward"),  "Next");
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
		if (mediaPlayer != nullptr)
			mediaPlayer->stateUpdated();
	});
	toolBar->addSeparator();
	toolBar->addWidget(progress);
	toolBar->addSeparator();
	position = new QLabel("0:00/0:00", this);
	toolBar->addWidget(position);
	toolBar->addSeparator();
	// Shuffle and repeat toggles
	shuffle = toolBar->addAction(Icon::get("media-playlist-shuffle"), "Shuffle");
	shuffle->setCheckable(true);
	QAction::connect(shuffle, &QAction::triggered, [=](bool checked) {
		auto status = spotify->setShuffle(checked);
		if (!status.isEmpty())
			setStatus(QString("Failed to toggle shuffle: %1").arg(status));
	});
	repeat = toolBar->addAction(Icon::get("media-playlist-repeat"), "Repeat");
	repeat->setCheckable(true);
	QAction::connect(repeat, &QAction::triggered, [=](bool checked) {
		auto status = spotify->setRepeat(checked ? "context" : "off");
		if (!status.isEmpty())
			setStatus(QString("Failed to toggle repeat: %1").arg(status));
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

void MainWindow::openAudioFeaturesWidget(const QString &trackId, const QString &artist, const QString &name)
{
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea,
		new AudioFeaturesView(*spotify, trackId, artist, name, this));
}

void MainWindow::openLyrics(const QString &artist, const QString &name)
{
	addDockWidget(Qt::DockWidgetArea::RightDockWidgetArea,
		new LyricsView(artist, name, this));
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
			"", track.name, track.artist, track.album,
			formatTime(track.duration), track.addedAt.toString("yyyy-MM-dd")
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
		if (track.id == current.item.id)
			item->setIcon(0, Icon::get("media-playback-start"));
		songs->insertTopLevelItem(i, item);
	}
	return true;
}

bool MainWindow::loadAlbum(const QString &albumId, bool ignoreEmpty)
{
	auto tracks = spotify->albumTracks(albumId);
	if (ignoreEmpty && tracks->length() <= 1)
		setStatus("Album only contains one song or is empty");
	else
	{
		playlists->setCurrentRow(-1);
		sptContext = QString("spotify:album:%1").arg(albumId);
		loadSongs(*tracks);
	}
	delete tracks;
	return true;
}

bool MainWindow::loadPlaylist(spt::Playlist &playlist)
{
	Settings().setLastPlaylist(playlist.id);
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
	auto filePath = QString("%1/playlist/%2").arg(cacheLocation).arg(playlistId);
	if (!QFileInfo::exists(filePath))
		return tracks;
	QFile file(filePath);
	file.open(QIODevice::ReadOnly);
	auto json = QJsonDocument::fromBinaryData(file.readAll(), QJsonDocument::BypassValidation);
	if (json.isNull())
		return tracks;
	for (auto track : json["tracks"].toArray())
		tracks.append(spt::Track(track.toObject()));
	return tracks;
}

void MainWindow::refreshPlaylist(spt::Playlist &playlist, bool force)
{
	auto newPlaylist = spotify->playlist(playlist.id);
	if (!force && playlist.snapshot == newPlaylist.snapshot)
		return;
	loadSongs(newPlaylist.loadTracks(*spotify));
	cachePlaylist(newPlaylist);
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
		if (item->data(0, RoleTrackId).toString() == QString("spotify:track:%1").arg(current.item.id))
			item->setIcon(0, Icon::get("media-playback-start"));
		else
			item->setIcon(0, QIcon());
	}
}

void MainWindow::setAlbumImage(const QString &url)
{
	nowAlbum->setPixmap(getAlbum(url));
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
	addDockWidget(
		Qt::DockWidgetArea::RightDockWidgetArea,
		new ArtistView(*spotify, artistId, this));
}

void MainWindow::cachePlaylist(spt::Playlist &playlist)
{
	QJsonDocument json(playlist.toJson(*spotify));
	QFile file(QString("%1/playlist/%2").arg(cacheLocation).arg(playlist.id));
	file.open(QIODevice::WriteOnly);
	file.write(json.toBinaryData());
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