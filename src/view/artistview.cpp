#include "artistview.hpp"

ArtistView::ArtistView(spt::Spotify &spotify, const QString &artistId,
	const lib::settings &settings, QWidget *parent)
	: spotify(spotify),
	artistId(artistId),
	QWidget(parent)
{
	spotify.artist(artistId, [this](const spt::Artist &loadedArtist)
	{
		artistLoaded(loadedArtist);
	});

	layout = new QVBoxLayout();
	layout->setContentsMargins(-1, 0, -1, 0);
	setLayout(layout);

	// Placeholder for cover  image
	coverLabel = new QLabel(this);
	coverLabel->setMinimumHeight(160);
	layout->addWidget(coverLabel);

	// Artist name title
	title = new QHBoxLayout();
	name = new QLabel("...", this);
	name->setWordWrap(true);
	new Loader(name);

	auto titleFont = name->font();
	titleFont.setPointSize(24);
	name->setFont(titleFont);
	title->addWidget(name, 1);

	// Context menu
	menu = new QMenu(this);

	popularity = menu->addAction(Icon::get("draw-donut"), "popularity");
	popularity->setEnabled(false);

	followButton = menu->addAction(Icon::get("non-starred-symbolic"), "Follow");
	followButton->setEnabled(false);
	QAction::connect(followButton, &QAction::triggered, this, &ArtistView::follow);

	auto menuSearch = menu->addMenu(Icon::get("edit-find"), "Search");
	QAction::connect(menuSearch->addAction("Wikipedia"), &QAction::triggered,
		this, &ArtistView::searchWikipedia);
	QAction::connect(menuSearch->addAction("DuckDuckGo"), &QAction::triggered,
		this, &ArtistView::searchDuckDuckGo);

	auto shareMenu = menu->addMenu(Icon::get("document-share"), "Share");
	QAction::connect(shareMenu->addAction("Copy artist link"), &QAction::triggered,
		this, &ArtistView::copyLink);
	QAction::connect(shareMenu->addAction("Open in Spotify"), &QAction::triggered,
		this, &ArtistView::openInSpotify);

	context = new QToolButton(this);
	context->setEnabled(false);
	context->setIcon(Icon::get("media-playback-start"));
	context->setMenu(menu);
	context->setPopupMode(QToolButton::MenuButtonPopup);
	QAbstractButton::connect(context, &QAbstractButton::clicked, this, &ArtistView::play);
	title->addWidget(context);
	layout->addLayout(title);

	// Genres
	genres = new QLabel(QString(), this);
	genres->setWordWrap(true);
	layout->addWidget(genres);

	// Tabs
	tabs = new QTabWidget(this);
	layout->addWidget(tabs);

	// Top tracks
	topTracksList = new QListWidget(tabs);
	topTracksList->setEnabled(false);
	topTracksList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QListWidget::connect(topTracksList, &QListWidget::itemActivated, this, &ArtistView::trackClick);
	QWidget::connect(topTracksList, &QWidget::customContextMenuRequested,
		this, &ArtistView::trackMenu);
	tabs->addTab(topTracksList, "Popular");

	// Albums
	albumList = new QTreeWidget(tabs);
	singleList = new QTreeWidget(tabs);

	for (auto &list : {albumList, singleList})
	{
		list->setEnabled(false);
		list->setColumnCount(2);
		list->header()->hide();
		list->setRootIsDecorated(false);
		list->header()->resizeSection(0, 235);
		list->header()->resizeSection(1, 1);
		list->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);

		QTreeWidget::connect(list, &QTreeWidget::itemClicked, this, &ArtistView::loadAlbumId);
		QWidget::connect(list, &QWidget::customContextMenuRequested, this, &ArtistView::albumMenu);
		QTreeWidget::connect(list, &QTreeWidget::itemDoubleClicked,
			this, &ArtistView::albumDoubleClicked);
	}

	tabs->addTab(albumList, "Albums");
	tabs->addTab(singleList, "Singles");

	// Related artists
	relatedList = new QListWidget(tabs);
	relatedList->setEnabled(false);
	QListWidget::connect(relatedList, &QListWidget::itemClicked, this, &ArtistView::relatedClick);
	tabs->addTab(relatedList, "Related");
}

void ArtistView::artistLoaded(const spt::Artist &loadedArtist)
{
	artist = loadedArtist;

	if (onArtistLoaded)
		onArtistLoaded(loadedArtist);

	auto mainWindow = MainWindow::find(parentWidget());

	// Get cover image (320x320 -> 320x160)
	QPixmap cover;
	cover.loadFromData(mainWindow->get(artist.image), "jpeg");
	coverLabel->setPixmap(cover.copy(0, 80, 320, 160));

	// Format followers
	char prefix = artist.followers > 1000000
		? 'M' : artist.followers > 1000
			? 'k' : '\0';
	auto followers = QString("%1%2 follower%3")
		.arg(prefix == 'M'
			? artist.followers / 1000000 : prefix == 'k'
				? artist.followers / 1000 : artist.followers)
		.arg(prefix)
		.arg(artist.followers == 1 ? "" : "s");

	// Artist name title
	name->setText(artist.name);

	// Menu actions
	popularity->setIcon(QIcon(Utils::mask(Icon::get("draw-donut")
		.pixmap(64, 64), MaskShape::Pie, QVariant(artist.popularity))));
	popularity->setText(QString("%1% popularity").arg(artist.popularity));

	followButton->setText(QString("Follow (%2)").arg(followers));

	spotify.isFollowing(FollowType::Artist, {
		artistId
	}, [this](const std::vector<bool> &follows)
	{
		updateFollow(follows.empty() ? false : follows[0]);
		this->followButton->setEnabled(true);
	});

	// Artist is loaded now at least
	context->setEnabled(true);

	// Genres
	genres->setText(artist.genres.join(", "));

	// Top tracks
	spotify.topTracks(artist, [this](const std::vector<spt::Track> &tracks)
	{
		topTracksLoaded(tracks);
	});

	// Albums
	spotify.albums(artist, [this](const std::vector<spt::Album> &albums)
	{
		albumsLoaded(albums);
	});

	// Related artists
	spotify.relatedArtists(artist, [this](const std::vector<spt::Artist> &artists)
	{
		relatedArtistsLoaded(artists);
	});
}

void ArtistView::topTracksLoaded(const std::vector<spt::Track> &tracks)
{
	auto mainWindow = MainWindow::find(parentWidget());
	auto i = 0;

	for (auto &track : tracks)
	{
		auto item = new QListWidgetItem(track.name, topTracksList);
		item->setIcon(QIcon(mainWindow->getAlbum(track.image)));
		item->setData(RoleTrackId, track.id);
		item->setData(RoleAlbumId, track.albumId);
		item->setData(RoleIndex, i++);
		topTrackIds.append(QString("spotify:track:%1").arg(track.id));
	}

	topTracksList->setEnabled(true);
}

void ArtistView::albumsLoaded(const std::vector<spt::Album> &albums)
{
	auto mainWindow = MainWindow::find(parentWidget());

	for (auto &album : albums)
	{
		auto parentTab = album.albumGroup == "single" ? singleList : albumList;
		auto year = album.releaseDate.toString("yyyy");
		auto item = new QTreeWidgetItem(parentTab, {
			album.name, year.isEmpty() ? QString() : year
		});

		item->setIcon(0, QIcon(mainWindow->getAlbum(album.image)));
		item->setData(0, RoleAlbumId, album.id);
		item->setToolTip(1, QLocale::system()
			.toString(album.releaseDate.date(), QLocale::FormatType::ShortFormat));
		parentTab->insertTopLevelItem(0, item);
	}

	albumList->setEnabled(true);
	singleList->setEnabled(true);
}

void ArtistView::relatedArtistsLoaded(const std::vector<spt::Artist> &artists)
{
	for (auto &related : artists)
	{
		auto item = new QListWidgetItem(related.name, relatedList);
		item->setData(RoleArtistId, related.id);
	}

	relatedList->setEnabled(true);
}

void ArtistView::updateFollow(bool isFollowing)
{
	followButton->setIcon(Icon::get(QString("%1starred-symbolic").arg(isFollowing ? "non-" : "")));
	followButton->setText(QString("%1%2")
		.arg(isFollowing ? "Unfollow" : "Follow")
		.arg(followButton->text()
			.right(followButton->text().length() - followButton->text().indexOf(' '))));
}

void ArtistView::follow(bool)
{
	auto isFollowing = followButton->text().contains("Unfollow");
	updateFollow(!isFollowing);

	if (isFollowing)
	{
		spotify.unfollow(FollowType::Artist, {
			artistId
		});
	}
	else
	{
		spotify.follow(FollowType::Artist, {
			artistId
		});
	}
}

void ArtistView::trackClick(QListWidgetItem *item)
{
	auto result = spotify.playTracks(item->data(RoleIndex).toInt(), topTrackIds);
	if (!result.isEmpty())
		MainWindow::find(parentWidget())
			->setStatus(QString("Failed to start playback: %1").arg(result), true);
}

void ArtistView::trackMenu(const QPoint &pos)
{
	auto item = topTracksList->itemAt(pos);
	auto trackId = item->data(RoleTrackId).toString();
	if (trackId.isEmpty())
		return;
	(new SongMenu(item, artist.name, spotify, parentWidget()))
		->popup(topTracksList->mapToGlobal(pos));
}

void ArtistView::loadAlbumId(QTreeWidgetItem *item)
{
	auto mainWindow = MainWindow::find(parentWidget());
	if (!mainWindow->loadAlbum(item->data(0, RoleAlbumId).toString()))
		mainWindow->setStatus(QString("Failed to load album"), true);
}

void ArtistView::relatedClick(QListWidgetItem *item)
{
	relatedList->setEnabled(false);
	MainWindow::find(parentWidget())->openArtist(item->data(RoleArtistId).toString());
	relatedList->setEnabled(true);
}

void ArtistView::albumMenu(const QPoint &pos)
{
	auto list = tabs->currentIndex() == 1
		? albumList
		: tabs->currentIndex() == 2
			? singleList
			: nullptr;
	if (list == nullptr)
		return;

	auto item = list->itemAt(pos);
	auto albumId = item->data(0, RoleAlbumId).toString();
	if (albumId.isEmpty())
		return;
	(new AlbumMenu(spotify, albumId, parentWidget()))->popup(list->mapToGlobal(pos));
}

void ArtistView::albumDoubleClicked(QTreeWidgetItem *item, int)
{
	auto mainWindow = MainWindow::find(parentWidget());
	auto result = spotify.playTracks(
		QString("spotify:album:%1").arg(item->data(0, RoleAlbumId).toString()));
	if (!result.isEmpty())
		mainWindow->setStatus(QString("Failed to start playlist playback: %1").arg(result), true);
}

void ArtistView::searchWikipedia(bool)
{
	Utils::openUrl(QString
		("https://www.wikipedia.org/search-redirect.php?family=wikipedia&go=Go&search=%1")
		.arg(artist.name), LinkType::Web, this);
}
void ArtistView::searchDuckDuckGo(bool)
{
	Utils::openUrl(QString("https://duckduckgo.com/?t=h_&q=%1").arg(artist.name),
		LinkType::Web, this);
}

void ArtistView::play(bool)
{
	spotify.playTracks(QString("spotify:artist:%1").arg(artistId));
}

void ArtistView::copyLink(bool)
{
	QApplication::clipboard()->setText(QString("https://open.spotify.com/artist/%1").arg(artistId));
	MainWindow::find(parentWidget())->setStatus("Link copied to clipboard");
}

void ArtistView::openInSpotify(bool)
{
	Utils::openUrl(QString("https://open.spotify.com/artist/%1").arg(artistId),
		LinkType::Web, MainWindow::find(parentWidget()));
}
