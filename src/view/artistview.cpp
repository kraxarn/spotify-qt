#include "artistview.hpp"

ArtistView::ArtistView(spt::Spotify &spotify, const QString &artistId, const Settings &settings, QWidget *parent)
	: spotify(spotify), artistId(artistId), QWidget(parent)
{
	auto mainWindow = MainWindow::find(parentWidget());
	if (mainWindow == nullptr)
	{
		Log::error("Parent is not MainWindow");
		return;
	}

	artist = spotify.artist(artistId);
	setWindowTitle(artist.name);
	auto layout = new QVBoxLayout();
	layout->setContentsMargins(-1, 0, -1, 0);
	setLayout(layout);

	// Get cover image (320x320 -> 320x160)
	QPixmap cover;
	cover.loadFromData(mainWindow->get(artist.image), "jpeg");
	auto coverLabel = new QLabel(this);
	coverLabel->setPixmap(cover.copy(0, 80, 320, 160));
	layout->addWidget(coverLabel);

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
	auto title = new QHBoxLayout();
	auto name = new QLabel(artist.name, this);
	name->setWordWrap(true);

	auto titleFont = name->font();
	titleFont.setPointSize(24);
	name->setFont(titleFont);
	title->addWidget(name, 1);

	auto menu = new QMenu(this);
	menu->addAction(QIcon(Utils::mask(Icon::get("draw-donut").pixmap(64, 64),
			MaskShape::Pie, QVariant(artist.popularity))),
			QString("%1% popularity").arg(artist.popularity))
		->setEnabled(false);

	auto follows = spotify.isFollowing(FollowType::Artist, {
		artistId
	});
	auto isFollowing = follows.isEmpty() ? false : follows[0];
	followButton = menu->addAction(Icon::get(QString("%1starred-symbolic")
			.arg(isFollowing ? "" : "non-")),
		QString("%1 (%2)").arg(isFollowing
			? "Unfollow"
			: "Follow").arg(followers));
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

	auto popularity = new QToolButton(this);
	popularity->setIcon(Icon::get("media-playback-start"));
	popularity->setMenu(menu);
	popularity->setPopupMode(QToolButton::MenuButtonPopup);
	QAbstractButton::connect(popularity, &QAbstractButton::clicked, this, &ArtistView::play);
	title->addWidget(popularity);
	layout->addLayout(title);

	// Genres
	auto genres = new QLabel(artist.genres.join(", "));
	genres->setWordWrap(true);
	layout->addWidget(genres);

	// Tabs
	tabs = new QTabWidget(this);
	layout->addWidget(tabs);

	// Top tracks
	auto topTracks = artist.topTracks(spotify);
	topTracksList = new QListWidget(tabs);
	auto i = 0;
	for (auto &track : topTracks)
	{
		auto item = new QListWidgetItem(track.name, topTracksList);
		item->setIcon(QIcon(mainWindow->getAlbum(track.image)));
		item->setData(RoleTrackId, track.id);
		item->setData(RoleAlbumId, track.albumId);
		item->setData(RoleIndex, i++);
		topTrackIds.append(QString("spotify:track:%1").arg(track.id));
	}
	QListWidget::connect(topTracksList, &QListWidget::itemActivated, this, &ArtistView::trackClick);

	topTracksList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(topTracksList, &QWidget::customContextMenuRequested, this, &ArtistView::trackMenu);
	tabs->addTab(topTracksList, "Popular");

	// Albums
	auto albums = artist.albums(spotify);
	albumList = new QTreeWidget(tabs);
	singleList = new QTreeWidget(tabs);
	for (auto &list : {albumList, singleList})
	{
		list->setColumnCount(2);
		list->header()->hide();
		list->setRootIsDecorated(false);
		list->header()->resizeSection(0, 235);
		list->header()->resizeSection(1, 1);
	}
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

	QVector<QTreeWidget *> lists({
		albumList, singleList
	});
	for (auto list : lists)
	{
		QTreeWidget::connect(list, &QTreeWidget::itemClicked, this, &ArtistView::loadAlbumId);

		list->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
		QWidget::connect(list, &QWidget::customContextMenuRequested, this, &ArtistView::albumMenu);
		QTreeWidget::connect(list, &QTreeWidget::itemDoubleClicked, this, &ArtistView::albumDoubleClicked);
	}

	tabs->addTab(albumList, "Albums");
	tabs->addTab(singleList, "Singles");

	// Related artists
	auto relatedArtists = artist.relatedArtists(spotify);
	relatedList = new QListWidget(tabs);
	for (auto &related : relatedArtists)
	{
		auto item = new QListWidgetItem(related.name, relatedList);
		item->setData(RoleArtistId, related.id);
	}
	QListWidget::connect(relatedList, &QListWidget::itemClicked, this, &ArtistView::relatedClick);
	tabs->addTab(relatedList, "Related");
}

void ArtistView::follow(bool)
{
	auto isFollowing = followButton->text().contains("Unfollow");
	followButton->setIcon(Icon::get(QString("%1starred-symbolic").arg(isFollowing ? "non-" : "")));
	followButton->setText(followButton->toolTip()
		.replace(isFollowing ? "Unfollow" : "Follow",
			isFollowing ? "Follow" : "Unfollow"));

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
	if (!mainWindow->loadAlbum(item->data(0, RoleAlbumId).toString(), false))
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
	Utils::openUrl(QString("https://www.wikipedia.org/search-redirect.php?family=wikipedia&go=Go&search=%1")
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
