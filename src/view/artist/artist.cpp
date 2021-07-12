#include "view/artist/artist.hpp"
#include "mainwindow.hpp"

View::Artist::Artist::Artist(lib::spt::api &spotify, const std::string &artistId,
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

	// Placeholder for cover image
	coverLabel = new View::Artist::Cover(this);
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
	context = new View::Artist::PlayButton(spotify, this);
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
	QListWidget::connect(topTracksList, &QListWidget::itemActivated, this,
		&View::Artist::Artist::trackClick);
	QWidget::connect(topTracksList, &QWidget::customContextMenuRequested,
		this, &View::Artist::Artist::trackMenu);
	tabs->addTab(topTracksList, "Popular");

	// Albums
	albumList = new View::Artist::Albums(spotify, cache, httpClient, this);
	tabs->addTab(albumList, "Discography");

	// Related artists
	relatedList = new QListWidget(tabs);
	relatedList->setEnabled(false);
	QListWidget::connect(relatedList, &QListWidget::itemClicked, this,
		&View::Artist::Artist::relatedClick);
	tabs->addTab(relatedList, "Related");

	spotify.artist(this->artistId, [this](const lib::spt::artist &loadedArtist)
	{
		artistLoaded(loadedArtist);
	});
}

void View::Artist::Artist::artistLoaded(const lib::spt::artist &loadedArtist)
{
	artist = loadedArtist;

	if (onArtistLoaded)
	{
		onArtistLoaded(loadedArtist);
	}

	// Get cover image
	httpClient.get(artist.image, lib::headers(), [this](const std::string &data)
	{
		coverLabel->setJpeg(QByteArray::fromStdString(data));
	});

	auto followers = QString("%1 follower%2")
		.arg(QString::fromStdString(lib::fmt::count(artist.followers)),
			artist.followers == 1 ? "" : "s");

	// Artist name title
	name->setText(QString::fromStdString(artist.name));

	// Artist is loaded now at least
	context->setArtist(artist);

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
		albumList->setAlbums(albums);
	});

	// Related artists
	spotify.related_artists(artist, [this](const std::vector<lib::spt::artist> &artists)
	{
		relatedArtistsLoaded(artists);
	});
}

void View::Artist::Artist::topTracksLoaded(const std::vector<lib::spt::track> &tracks)
{
	auto i = 0;
	for (const auto &track : tracks)
	{
		auto *item = new QListWidgetItem(QString::fromStdString(track.name), topTracksList);

		HttpUtils::getAlbum(track.image, httpClient, cache, [item](const QPixmap &image)
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

void View::Artist::Artist::relatedArtistsLoaded(const std::vector<lib::spt::artist> &artists)
{
	for (const auto &related : artists)
	{
		auto *item = new QListWidgetItem(QString::fromStdString(related.name), relatedList);
		item->setData(RoleArtistId, QString::fromStdString(related.id));
	}

	relatedList->setEnabled(true);
}

void View::Artist::Artist::trackClick(QListWidgetItem *item)
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

void View::Artist::Artist::trackMenu(const QPoint &pos)
{
	auto *item = topTracksList->itemAt(pos);
	const auto &track = item->data(RoleTrack).value<lib::spt::track>();
	if (!track.is_valid())
	{
		return;
	}

	auto *songMenu = new SongMenu(track, spotify, cache, &artist, parentWidget());
	songMenu->popup(topTracksList->mapToGlobal(pos));
}

void View::Artist::Artist::relatedClick(QListWidgetItem *item)
{
	relatedList->setEnabled(false);
	MainWindow::find(parentWidget())->openArtist(item->data(RoleArtistId)
		.toString().toStdString());
	relatedList->setEnabled(true);
}
