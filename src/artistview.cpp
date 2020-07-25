#include "artistview.hpp"

ArtistView::ArtistView(spt::Spotify &spotify, const QString &artistId, QWidget *parent)
	: spotify(spotify), artistId(artistId), QDockWidget(parent)
{
	this->parent = dynamic_cast<MainWindow *>(parent);
	if (this->parent == nullptr)
		return;
	auto mainWindow = (MainWindow*) parent;

	artist = spotify.artist(artistId);
	setFeatures(QDockWidget::DockWidgetClosable);
	auto layout = new QVBoxLayout();
	layout->setContentsMargins(-1, 0, -1, 0);

	// Get cover image (320x320 -> 320x160)
	QPixmap cover;
	cover.loadFromData(mainWindow->get(artist.image), "jpeg");
	auto coverLabel = new QLabel(this);
	coverLabel->setPixmap(cover.copy(0, 80, 320, 160));
	layout->addWidget(coverLabel);

	// Format followers
	char prefix =
		artist.followers > 1000000
		? 'M'
		: artist.followers > 1000
		  ? 'k'
		  : '\0';
	auto followers = QString("%1%2 follower%3")
		.arg(prefix == 'M'
			 ? artist.followers / 1000000
			 : prefix == 'k'
			   ? artist.followers / 1000
			   : artist.followers)
		.arg(prefix)
		.arg(artist.followers == 1 ? "" : "s");

	// Artist name title
	auto isFollowing = spotify.isFollowing(spt::Spotify::FollowType::Artist, {artistId})[0];
	auto title = new QHBoxLayout();
	followButton = new QPushButton(this);
	followButton->setIcon(Icon::get(QString("%1starred-symbolic").arg(isFollowing ? "" : "non-")));
	followButton->setToolTip(
		QString("%1 artist (%2)")
			.arg(isFollowing
				 ? "UnfollowButton"
				 : "Follow").arg(followers));
	followButton->setFlat(true);
	QAbstractButton::connect(followButton, &QAbstractButton::clicked, this, &ArtistView::follow);
	title->addWidget(followButton);

	auto name = new QLabel(artist.name, this);
	name->setAlignment(Qt::AlignHCenter);
	name->setWordWrap(true);

	auto titleFont = name->font();
	titleFont.setPointSize(24);
	name->setFont(titleFont);
	title->addWidget(name, 1);

	auto popularityText = QString("%1% popularity").arg(artist.popularity);
	auto popularity = new QPushButton(this);
	popularity->setToolTip(popularityText);
	popularity->setIcon(QIcon(Utils::mask(
		Icon::get("draw-donut").pixmap(64, 64),
		Utils::MaskShape::Pie,
		QVariant(artist.popularity))));
	popularity->setFlat(true);
	QAbstractButton::connect(popularity, &QAbstractButton::clicked, [popularity, popularityText](bool checked)
	{
		QToolTip::showText(QCursor::pos(), popularityText);
	});
	title->addWidget(popularity);
	layout->addLayout(title);

	// Genres
	auto genres = new QLabel(artist.genres.join(", "));
	genres->setWordWrap(true);
	layout->addWidget(genres);

	// Tabs
	auto tabs = new QTabWidget(this);
	layout->addWidget(tabs);

	// Top tracks
	auto topTracks = artist.topTracks(spotify);
	topTracksList = new QListWidget(tabs);
	for (auto &track : topTracks)
	{
		auto item = new QListWidgetItem(track.name, topTracksList);
		item->setIcon(QIcon(mainWindow->getAlbum(track.image)));
		item->setData(Utils::RoleTrackId, track.id);
		item->setData(Utils::RoleAlbumId, track.albumId);
		topTrackIds.append(QString("spotify:track:%1").arg(track.id));
	}
	QListWidget::connect(topTracksList, &QListWidget::itemClicked, this, &ArtistView::trackClick);

	topTracksList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(topTracksList, &QWidget::customContextMenuRequested, this, &ArtistView::trackMenu);
	tabs->addTab(topTracksList, "Popular");

	// Albums
	auto albums = artist.albums(spotify);
	auto albumList = new QTreeWidget(tabs);
	auto singleList = new QTreeWidget(tabs);
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
			album.name,
			year.isEmpty() ? QString() : year
		});
		item->setIcon(0, QIcon(mainWindow->getAlbum(album.image)));
		item->setData(0, Utils::RoleAlbumId, album.id);
		parentTab->insertTopLevelItem(0, item);
	}

	QVector<QTreeWidget *> lists(
		{
			albumList, singleList
		}
	);
	for (auto list : lists)
	{
		QTreeWidget::connect(list, &QTreeWidget::itemClicked, this, &ArtistView::loadAlbumId);

		list->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
		QWidget::connect(list, &QWidget::customContextMenuRequested, this, &ArtistView::albumMenu);
	}

	tabs->addTab(albumList, "Albums");
	tabs->addTab(singleList, "Singles");

	// Related artists
	auto relatedArtists = artist.relatedArtists(spotify);
	relatedList = new QListWidget(tabs);
	for (auto &related : relatedArtists)
	{
		auto item = new QListWidgetItem(related.name, relatedList);
		item->setData(Utils::RoleArtistId, related.id);
	}
	QListWidget::connect(relatedList, &QListWidget::itemClicked, this, &ArtistView::relatedClick);
	tabs->addTab(relatedList, "Related");

	// Rest of dock
	setWidget(Utils::layoutToWidget(layout));
	setFixedWidth(320);
}

void ArtistView::follow(bool)
{
	auto isFollowing = followButton->toolTip().contains("Unfollow");
	followButton->setIcon(Icon::get(QString("%1starred-symbolic").arg(isFollowing ? "non-" : "")));
	followButton->setToolTip(followButton->toolTip().replace(
		isFollowing ? "Unfollow" : "Follow",
		isFollowing ? "Follow" : "Unfollow"));
	if (isFollowing)
		spotify.unfollow(spt::Spotify::FollowType::Artist, {artistId});
	else
		spotify.follow(spt::Spotify::FollowType::Artist, {artistId});
}

void ArtistView::trackClick(QListWidgetItem *item)
{
	auto result = spotify.playTracks(
		QString("spotify:track:%1").arg(item->data(Utils::RoleTrackId).toString()),
		topTrackIds);
	if (!result.isEmpty())
		((MainWindow*) parent)->setStatus(QString("Failed to start playback: %1").arg(result), true);
}

void ArtistView::trackMenu(const QPoint &pos)
{
	auto item = topTracksList->itemAt(pos);
	auto trackId = item->data(Utils::RoleTrackId).toString();
	if (trackId.isEmpty())
		return;
	(new SongMenu(trackId, artist.name, item->text(), artist.id,
				  item->data(Utils::RoleAlbumId).toString(), spotify, parent))
		->popup(topTracksList->mapToGlobal(pos));
}

void ArtistView::loadAlbumId(QTreeWidgetItem *item)
{
	auto mainWindow = (MainWindow*) parent;
	if (!mainWindow->loadAlbum(item->data(0, Utils::RoleAlbumId).toString(), false))
		mainWindow->setStatus(QString("Failed to load album"), true);
}

void ArtistView::relatedClick(QListWidgetItem *item)
{
	relatedList->setEnabled(false);
	((MainWindow*) parent)->openArtist(item->data(Utils::RoleArtistId).toString());
}

void ArtistView::albumMenu(const QPoint &pos)
{
	auto item = topTracksList->itemAt(pos);
	auto albumId = item->data(Utils::RoleAlbumId).toString();
	if (albumId.isEmpty())
		return;
	(new AlbumMenu(spotify, albumId, parent))->popup(topTracksList->mapToGlobal(pos));
}
