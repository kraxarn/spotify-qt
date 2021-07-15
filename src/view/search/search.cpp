#include "view/search/search.hpp"
#include "mainwindow.hpp"

View::Search::Search::Search(lib::spt::api &spotify, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	httpClient(httpClient),
	QWidget(parent)
{
	auto *layout = new QVBoxLayout();
	layout->setContentsMargins(-1, 0, -1, 0);
	searchBox = new QLineEdit(this);
	layout->addWidget(searchBox);
	setLayout(layout);

	// Tab container
	tabs = new QTabWidget(this);
	layout->addWidget(tabs);

	// Tab content
	artists = new View::Search::Artists(this);
	playlists = new View::Search::Playlists(spotify, cache, this);
	tracks = new View::Search::Tracks(spotify, cache, this);
	albums = new View::Search::Albums(spotify, cache, httpClient, this);

	// Add all tabs
	tabs->addTab(tracks, "Tracks");
	tabs->addTab(artists, "Artists");
	tabs->addTab(albums, "Albums");
	tabs->addTab(playlists, "Playlists");

	// Start searching when pressing enter
	QLineEdit::connect(searchBox, &QLineEdit::returnPressed,
		this, &View::Search::Search::search);
}

void View::Search::Search::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);
	MainWindow::find(parentWidget())->setSearchChecked(true);
	searchBox->setFocus();
}

void View::Search::Search::hideEvent(QHideEvent *event)
{
	QWidget::hideEvent(event);

	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		mainWindow->setSearchChecked(false);
	}
}

void View::Search::Search::search()
{
	// Empty all previous results
	tracks->clear();
	artists->clear();
	albums->clear();
	playlists->clear();

	// Disable search box while searching
	searchBox->setEnabled(false);

	// Check if spotify uri
	auto searchText = searchBox->text();

	if (searchText.startsWith("spotify:")
		|| searchText.startsWith("https://open.spotify.com/"))
	{
		auto parts = searchText.startsWith("https://open.spotify.com/")
			? searchText.right(searchText.length() - 8).split('/')
			: searchText.split(':');

		if (parts.length() >= 3)
		{
			auto i = tabs->currentIndex();
			auto cat = parts[1];
			auto id = parts[2].toStdString();

			if (cat == "track")
			{
				spotify.track(id, [this](const lib::spt::track &track)
				{
					this->tracks->add(track);
				});
				i = tracksIndex;
			}
			else if (cat == "artist")
			{
				spotify.artist(id, [this](const lib::spt::artist &artist)
				{
					this->artists->add(artist);
				});
				i = artistsIndex;
			}
			else if (cat == "album")
			{
				spotify.album(id, [this](const lib::spt::album &album)
				{
					this->albums->add(album);
				});
				i = albumsIndex;
			}
			else if (cat == "playlist")
			{
				spotify.playlist(id, [this](const lib::spt::playlist &playlist)
				{
					this->playlists->add(playlist);
				});
				i = playlistsIndex;
			}

			tabs->setCurrentIndex(i);
		}
	}
	else
	{
		// Get search results
		spotify.search(searchText.toStdString(),
			[this](const lib::spt::search_results &results)
			{
				this->resultsLoaded(results);
			});
	}
}

void View::Search::Search::resultsLoaded(const lib::spt::search_results &results)
{
	// Albums
	for (const auto &album : results.albums)
	{
		albums->add(album);
	}

	// Artists
	for (const auto &artist : results.artists)
	{
		artists->add(artist);
	}

	// Playlists
	for (const auto &playlist : results.playlists)
	{
		playlists->add(playlist);
	}

	// Tracks
	for (const auto &track : results.tracks)
	{
		tracks->add(track);
	}

	// Search done
	searchBox->setEnabled(true);
}
