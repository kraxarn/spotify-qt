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
	topTracksList = new View::Artist::TracksList(spotify, cache,
		httpClient, artist, tabs);
	tabs->addTab(topTracksList, "Popular");

	// Albums
	albumList = new View::Artist::AlbumsList(spotify, cache, httpClient, this);
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
	for (const auto &track : tracks)
	{
		topTracksList->addTrack(track);
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

void View::Artist::Artist::relatedClick(QListWidgetItem *item)
{
	relatedList->setEnabled(false);
	MainWindow::find(parentWidget())->openArtist(item->data(RoleArtistId)
		.toString().toStdString());
	relatedList->setEnabled(true);
}
