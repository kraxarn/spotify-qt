#include "artistview.hpp"

ArtistView::ArtistView(spt::Spotify &spotify, const std::string &artistId,
	lib::cache &cache, const lib::http_client &httpClient, QWidget *parent)
	: spotify(spotify),
	artistId(std::string(artistId)),
	cache(cache),
	httpClient(httpClient),
	QWidget(parent)
{
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

	constexpr int titleFontSize = 24;
	auto titleFont = name->font();
	titleFont.setPointSize(titleFontSize);
	name->setFont(titleFont);
	title->addWidget(name, 1);

	// Context menu
	menu = new QMenu(this);

	popularity = menu->addAction(Icon::get("draw-donut"), "popularity");
	popularity->setEnabled(false);

	followButton = menu->addAction(Icon::get("non-starred-symbolic"), "Follow");
	followButton->setEnabled(false);
	QAction::connect(followButton, &QAction::triggered, this, &ArtistView::follow);

	auto *menuSearch = menu->addMenu(Icon::get("edit-find"), "Search");
	QAction::connect(menuSearch->addAction("Wikipedia"), &QAction::triggered,
		this, &ArtistView::searchWikipedia);
	QAction::connect(menuSearch->addAction("DuckDuckGo"), &QAction::triggered,
		this, &ArtistView::searchDuckDuckGo);

	auto *shareMenu = menu->addMenu(Icon::get("document-share"), "Share");
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

	for (const auto &list : {albumList, singleList})
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

	spotify.artist(this->artistId, [this](const lib::spt::artist &loadedArtist)
	{
		artistLoaded(loadedArtist);
	});
}

void ArtistView::artistLoaded(const lib::spt::artist &loadedArtist)
{
	artist = loadedArtist;

	if (onArtistLoaded)
	{
		onArtistLoaded(loadedArtist);
	}

	auto *mainWindow = MainWindow::find(parentWidget());

	// Get cover image (320x320 -> 320x160)
	QPixmap cover;
	cover.loadFromData(mainWindow->get(QString::fromStdString(artist.image)),
		"jpeg");
	coverLabel->setPixmap(cover.copy(0, 80, 320, 160));

	auto followers = QString("%1 follower%2")
		.arg(QString::fromStdString(lib::fmt::count(artist.followers)),
			artist.followers == 1 ? "" : "s");

	// Artist name title
	name->setText(QString::fromStdString(artist.name));

	// Menu actions
	popularity->setIcon(QIcon(ImageUtils::mask(Icon::get("draw-donut")
		.pixmap(64, 64), MaskShape::Pie, QVariant(artist.popularity))));
	popularity->setText(QString("%1% popularity").arg(artist.popularity));

	followButton->setText(QString("Follow (%2)").arg(followers));

	spotify.is_following(lib::follow_type::artist, {
		artistId
	}, [this](const std::vector<bool> &follows)
	{
		updateFollow(!follows.empty() && follows[0]);
		this->followButton->setEnabled(true);
	});

	// Artist is loaded now at least
	context->setEnabled(true);

	// Genres
	genres->setText(QString::fromStdString(lib::strings::join(artist.genres, ", ")));

	// Top tracks
	spotify.top_tracks(artist, [this](const std::vector<lib::spt::track> &tracks)
	{
		topTracksLoaded(tracks);
	});

	// Albums
	spotify.albums(artist, [this](const std::vector<lib::spt::album> &albums)
	{
		albumsLoaded(albums);
	});

	// Related artists
	spotify.related_artists(artist, [this](const std::vector<lib::spt::artist> &artists)
	{
		relatedArtistsLoaded(artists);
	});
}

void ArtistView::topTracksLoaded(const std::vector<lib::spt::track> &tracks)
{
	auto i = 0;
	for (const auto &track : tracks)
	{
		auto *item = new QListWidgetItem(QString::fromStdString(track.name), topTracksList);

		HttpUtils::getAlbum(httpClient, track.image, cache, [item](const QPixmap &image)
		{
			if (item != nullptr)
			{
				item->setIcon(QIcon(image));
			}
		});

		item->setData(RoleTrack, QVariant::fromValue(track));
		item->setData(RoleAlbumId, QString::fromStdString(track.album.id));
		item->setData(RoleIndex, i++);
		topTrackIds.push_back(lib::spt::api::to_uri("track", track.id));
	}

	topTracksList->setEnabled(true);
}

void ArtistView::albumsLoaded(const std::vector<lib::spt::album> &albums)
{
	for (const auto &album : albums)
	{
		auto releaseDate = QDateTime::fromString(QString::fromStdString(album.release_date),
			Qt::ISODate);
		auto *parentTab = album.album_group == lib::album_group::single
			? singleList
			: albumList;
		auto year = releaseDate.toString("yyyy");
		auto *item = new QTreeWidgetItem(parentTab, {
			QString::fromStdString(album.name),
			year.isEmpty() ? QString() : year
		});

		HttpUtils::getAlbum(httpClient, album.image, cache, [item](const QPixmap &image)
		{
			if (item != nullptr)
			{
				item->setIcon(0, QIcon(image));
			}
		});

		item->setData(0, RoleAlbumId, QString::fromStdString(album.id));
		item->setToolTip(1, QLocale::system()
			.toString(releaseDate.date(), QLocale::FormatType::ShortFormat));
		parentTab->insertTopLevelItem(0, item);
	}

	albumList->setEnabled(true);
	singleList->setEnabled(true);
}

void ArtistView::relatedArtistsLoaded(const std::vector<lib::spt::artist> &artists)
{
	for (const auto &related : artists)
	{
		auto *item = new QListWidgetItem(QString::fromStdString(related.name), relatedList);
		item->setData(RoleArtistId, QString::fromStdString(related.id));
	}

	relatedList->setEnabled(true);
}

void ArtistView::updateFollow(bool isFollowing)
{
	followButton->setIcon(Icon::get(QString("%1starred-symbolic")
		.arg(isFollowing ? "" : "non-")));

	followButton->setText(QString("%1%2")
		.arg(isFollowing ? "Unfollow" : "Follow", followButton->text()
			.right(followButton->text().length() - followButton->text().indexOf(' '))));
}

void ArtistView::follow(bool /*checked*/)
{
	auto isFollowing = followButton->text().contains("Unfollow");
	updateFollow(!isFollowing);

	auto callback = [this, isFollowing](const std::string &status)
	{
		if (status.empty())
		{
			return;
		}

		auto *mainWindow = MainWindow::find(this->parentWidget());
		mainWindow->status(lib::fmt::format("Failed to {}: {}",
			isFollowing ? "unfollow" : "follow", status));
	};

	if (isFollowing)
	{
		spotify.unfollow(lib::follow_type::artist, {
			artistId
		}, callback);
	}
	else
	{
		spotify.follow(lib::follow_type::artist, {
			artistId
		}, callback);
	}
}

void ArtistView::trackClick(QListWidgetItem *item)
{
	spotify.play_tracks(item->data(RoleIndex).toInt(), topTrackIds,
		[this](const std::string &result)
		{
			if (result.empty())
			{
				return;
			}

			auto *mainWindow = MainWindow::find(this->parentWidget());
			mainWindow->status(lib::fmt::format("Failed to start playback: {}",
				result), true);
		});
}

void ArtistView::trackMenu(const QPoint &pos)
{
	auto *item = topTracksList->itemAt(pos);
	const auto &track = item->data(RoleTrack).value<lib::spt::track>();
	if (!track.is_valid())
	{
		return;
	}

	auto *songMenu = new SongMenu(track, spotify, &artist, parentWidget());
	songMenu->popup(topTracksList->mapToGlobal(pos));
}

void ArtistView::loadAlbumId(QTreeWidgetItem *item)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	if (!mainWindow->loadAlbum(item->data(0, RoleAlbumId)
		.toString().toStdString()))
	{
		mainWindow->setStatus(QString("Failed to load album"), true);
	}
}

void ArtistView::relatedClick(QListWidgetItem *item)
{
	relatedList->setEnabled(false);
	MainWindow::find(parentWidget())->openArtist(item->data(RoleArtistId)
		.toString().toStdString());
	relatedList->setEnabled(true);
}

void ArtistView::albumMenu(const QPoint &pos)
{
	auto *list = tabs->currentIndex() == 1
		? albumList
		: tabs->currentIndex() == 2
			? singleList
			: nullptr;
	if (list == nullptr)
	{
		return;
	}

	auto *item = list->itemAt(pos);
	auto albumId = item->data(0, RoleAlbumId).toString();
	if (albumId.isEmpty())
	{
		return;
	}

	auto *albumMenu = new AlbumMenu(spotify, cache, albumId.toStdString(),
		parentWidget());
	albumMenu->popup(list->mapToGlobal(pos));
}

void ArtistView::albumDoubleClicked(QTreeWidgetItem *item, int /*column*/)
{
	spotify.play_tracks(lib::spt::api::to_uri("album",
		item->data(0, RoleAlbumId).toString().toStdString()),
		[this](const std::string &result)
		{
			if (result.empty())
			{
				return;
			}

			auto *mainWindow = MainWindow::find(this->parentWidget());
			mainWindow->status(lib::fmt::format("Failed to start playback: {}",
				result), true);
		});
}

void ArtistView::searchWikipedia(bool /*checked*/)
{
	UrlUtils::open(lib::fmt::format(
		"https://www.wikipedia.org/search-redirect.php?family=wikipedia&go=Go&search={}",
		artist.name), LinkType::Web, this);
}
void ArtistView::searchDuckDuckGo(bool /*checked*/)
{
	UrlUtils::open(lib::fmt::format("https://duckduckgo.com/?t=h_&q={}", artist.name),
		LinkType::Web, this);
}

void ArtistView::play(bool /*checked*/)
{
	spotify.play_tracks(lib::spt::api::to_uri("artist", artistId), {});
}

void ArtistView::copyLink(bool /*checked*/)
{
	QApplication::clipboard()->setText(QString("https://open.spotify.com/artist/%1")
		.arg(QString::fromStdString(artistId)));
	MainWindow::find(parentWidget())->setStatus("Link copied to clipboard");
}

void ArtistView::openInSpotify(bool /*chekced*/)
{
	UrlUtils::open(lib::fmt::format("https://open.spotify.com/artist/{}", artistId),
		LinkType::Web, MainWindow::find(parentWidget()));
}
