#include "view/artist/view.hpp"
#include "mainwindow.hpp"

Artist::View::View(lib::spt::api &spotify, const std::string &artistId, lib::cache &cache,
	const lib::http_client &httpClient, lib::settings &settings, QWidget *parent)
	: QWidget(parent),
	artistId(std::string(artistId)),
	spotify(spotify),
	cache(cache),
	httpClient(httpClient)
{
	layout = new QVBoxLayout();
	layout->setContentsMargins(-1, 0, -1, 0);
	setLayout(layout);

	// Placeholder for cover image
	coverLabel = new Artist::Cover(this);
	layout->addWidget(coverLabel);

	// Artist name title
	title = new QHBoxLayout();
	name = new Artist::Title(this);
	title->addWidget(name, 1);

	// Context menu
	context = new Artist::PlayButton(spotify, httpClient, this);
	title->addWidget(context);
	layout->addLayout(title);

	// Genres
	genres = new QLabel(QString(), this);
	genres->setWordWrap(true);
	genres->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
	layout->addWidget(genres);

	// Tabs
	tabs = new QTabWidget(this);
	layout->addWidget(tabs);

	// Top tracks
	topTracksList = new Artist::TracksList(spotify, cache, httpClient, artist, settings, tabs);
	tabs->addTab(topTracksList, "Popular");

	// Albums
	albumList = new Artist::AlbumsList(spotify, cache, httpClient, settings, this);
	tabs->addTab(albumList, "Discography");

	spotify.artist(this->artistId, [this](const lib::spt::artist &loadedArtist)
	{
		artistLoaded(loadedArtist);
	});
}

auto Artist::View::getArtist() const -> const lib::spt::artist &
{
	return artist;
}

void Artist::View::artistLoaded(const lib::spt::artist &loadedArtist)
{
	artist = loadedArtist;

	auto *sidePanel = Parent::findWidget<SidePanel::View>(parentWidget());
	if (sidePanel != nullptr)
	{
		sidePanel->setTabText(this, QString::fromStdString(artist.name));
	}

	// Get cover image
	if (artist.image.empty())
	{
		coverLabel->setJpeg({});
	}
	else
	{
		httpClient.get(artist.image, lib::headers(),
			[this](const std::string &data)
			{
				coverLabel->setJpeg(QByteArray::fromStdString(data));
			});
	}

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
	const std::vector<lib::album_group> groups{
		lib::album_group::album,
		lib::album_group::single,
		lib::album_group::compilation,
	};

	spotify.albums(artist, groups, [this](const lib::result<lib::spt::page<lib::spt::album>> &result) -> bool
	{
		if (!result.success())
		{
			StatusMessage::error(QStringLiteral("Failed to fetch artist albums: %1")
				.arg(QString::fromStdString(result.message())));
			return false;
		}

		const auto &page = result.value();
		albumList->loadAlbums(page);
		return page.has_next();
	});

	spotify.albums(artist, {lib::album_group::appears_on},
		[this](const lib::result<lib::spt::page<lib::spt::album>> &result) -> bool
		{
			if (!result.success())
			{
				lib::log::error("Failed to fetch appears on albums: {}", result.message());
				return false;
			}

			const auto &page = result.value();
			albumList->addAlbums(page.items);
			return false;
		});

	// Related artists
	spotify.related_artists(artist, [this](const std::vector<lib::spt::artist> &artists)
	{
		relatedArtistsLoaded(artists);
	});
}

void Artist::View::topTracksLoaded(const std::vector<lib::spt::track> &tracks)
{
	for (const auto &track: tracks)
	{
		topTracksList->addTrack(track);
	}
	topTracksList->setEnabled(true);
}

void Artist::View::relatedArtistsLoaded(const std::vector<lib::spt::artist> &artists)
{
	// Related artists are deprecated and not always available,
	// lazily load tab instead in case it isn't

	relatedList = new QListWidget(tabs);

	connect(relatedList, &QListWidget::itemClicked,
		this, &View::relatedClick);

	tabs->addTab(relatedList, "Related");

	for (const auto &related: artists)
	{
		auto *item = new QListWidgetItem(QString::fromStdString(related.name), relatedList);
		item->setData(static_cast<int>(DataRole::ArtistId),
			QString::fromStdString(related.id));
	}
}

void Artist::View::relatedClick(QListWidgetItem *item)
{
	relatedList->setEnabled(false);

	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->openArtist(item->data(static_cast<int>(DataRole::ArtistId))
		.toString().toStdString());

	relatedList->setEnabled(true);
}
