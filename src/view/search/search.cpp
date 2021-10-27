#include "view/search/search.hpp"
#include "mainwindow.hpp"

View::Search::Search::Search(lib::spt::api &spotify, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: QWidget(parent),
	spotify(spotify),
	cache(cache),
	httpClient(httpClient)
{
	auto *layout = new QVBoxLayout();
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
	library = new View::Search::Library(spotify, cache, this);
	shows = new View::Search::Shows(spotify, this);

	// Add all tabs
	tabs->addTab(tracks, "Tracks");
	tabs->addTab(artists, "Artists");
	tabs->addTab(albums, "Albums");
	tabs->addTab(playlists, "Playlists");
	tabs->addTab(shows, "Podcasts");
	tabs->addTab(library, "Library");

	// Start searching when pressing enter
	QLineEdit::connect(searchBox, &QLineEdit::returnPressed,
		this, &View::Search::Search::search);

	// Searching in library is a separate request,
	// so only actually search once requested
	QTabWidget::connect(tabs, &QTabWidget::currentChanged,
		this, &View::Search::Search::onIndexChanged);
}

void View::Search::Search::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);

	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		mainWindow->setSearchChecked(true);
	}

	searchBox->setFocus();
	searchBox->selectAll();
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
	library->clear();
	shows->clear();

	// Disable search box while searching
	searchBox->setEnabled(false);

	// Save last searched query
	searchText = searchBox->text();

	// Search in library cache until tab is selected
	library->searchCache(searchText.toStdString());

	// Don't actually search if nothing to search on
	if (searchText.isEmpty())
	{
		searchBox->setEnabled(true);
		return;
	}

	// Check if spotify uri
	if (searchText.startsWith("spotify:")
		|| searchText.startsWith("https://open.spotify.com/"))
	{
		// Length of "https://"
		constexpr int protocolLength = 8;

		auto parts = searchText.startsWith("https://open.spotify.com/")
			? searchText.right(searchText.length() - protocolLength).split('/')
			: searchText.split(':');

		if (parts.length() >= 3)
		{
			auto i = static_cast<SearchTab>(tabs->currentIndex());
			auto cat = parts[1];
			auto id = parts[2].toStdString();

			if (cat == "track")
			{
				spotify.track(id, [this](const lib::spt::track &track)
				{
					this->tracks->add(track);
				});
				i = SearchTab::Tracks;
			}
			else if (cat == "artist")
			{
				spotify.artist(id, [this](const lib::spt::artist &artist)
				{
					this->artists->add(artist);
				});
				i = SearchTab::Artists;
			}
			else if (cat == "album")
			{
				spotify.album(id, [this](const lib::spt::album &album)
				{
					this->albums->add(album);
				});
				i = SearchTab::Albums;
			}
			else if (cat == "playlist")
			{
				spotify.playlist(id, [this](const lib::spt::playlist &playlist)
				{
					this->playlists->add(playlist);
				});
				i = SearchTab::Playlists;
			}
			else if (cat == "show")
			{
				spotify.show(id, [this](const lib::spt::show &show)
				{
					shows->add(show);
				});
				i = SearchTab::Shows;
			}

			tabs->setCurrentIndex(static_cast<int>(i));
			searchBox->setEnabled(true);
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

		// Library search is handled separately
		if (static_cast<SearchTab>(tabs->currentIndex()) == SearchTab::Library)
		{
			library->search(searchText.toStdString());
		}
	}
}

void View::Search::Search::resultsLoaded(const lib::spt::search_results &results)
{
	// Albums
	for (const auto &album: results.albums)
	{
		albums->add(album);
	}

	// Artists
	for (const auto &artist: results.artists)
	{
		artists->add(artist);
	}

	// Playlists
	for (const auto &playlist: results.playlists)
	{
		playlists->add(playlist);
	}

	// Tracks
	for (const auto &track: results.tracks)
	{
		tracks->add(track);
	}

	// Shows/podcasts
	for (const auto &show: results.shows)
	{
		shows->add(show);
	}

	// Search done
	searchBox->setEnabled(true);
}

void View::Search::Search::onIndexChanged(int index)
{
	if (static_cast<SearchTab>(index) == SearchTab::Library)
	{
		library->search(searchText.toStdString());
	}
}
