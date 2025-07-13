#include "view/search/view.hpp"
#include "mainwindow.hpp"

Search::View::View(lib::spt::api &spotify, lib::cache &cache, const lib::http_client &httpClient,
	lib::settings &settings, QWidget *parent)
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
	tabs->setUsesScrollButtons(true);
	layout->addWidget(tabs);

	// Tab content
	artists = new Search::Artists(this);
	playlists = new Search::Playlists(spotify, cache, httpClient, settings, this);
	tracks = new Search::Tracks(spotify, cache, settings, httpClient, this);
	albums = new Search::Albums(spotify, cache, httpClient, settings, this);
	library = new Search::Library(spotify, cache, settings, httpClient, this);
	shows = new Search::Shows(spotify, this);

	// Add all tabs
	tabs->addTab(tracks, "Tracks");
	tabs->addTab(artists, "Artists");
	tabs->addTab(albums, "Albums");
	tabs->addTab(playlists, "Playlists");
	tabs->addTab(shows, "Podcasts");
	tabs->addTab(library, "Library");

	// Set up search timer for debounced live search
	searchTimer = new QTimer(this);
	searchTimer->setSingleShot(true);
	searchTimer->setInterval(250); // 500ms delay after user stops typing
	QTimer::connect(searchTimer, &QTimer::timeout, this, &Search::View::search);

	// Start searching when pressing enter (immediate search)
	QLineEdit::connect(searchBox, &QLineEdit::returnPressed, [this]()
	{
		// Stop timer and search immediately when Enter is pressed
		searchTimer->stop();
		search();
	});

	// Start live search when text changes
	QLineEdit::connect(searchBox, &QLineEdit::textChanged,
		this, &Search::View::onSearchTextChanged);

	// Searching in library is a separate request,
	// so only actually search once requested
	QTabWidget::connect(tabs, &QTabWidget::currentChanged,
		this, &Search::View::onIndexChanged);
}

void Search::View::showEvent(QShowEvent *event)
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

void Search::View::hideEvent(QHideEvent *event)
{
	QWidget::hideEvent(event);

	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		mainWindow->setSearchChecked(false);
	}
}

void Search::View::search()
{
	// Stop any pending timer-based search since we're searching now
	searchTimer->stop();

	// Empty all previous results
	tracks->clear();
	artists->clear();
	albums->clear();
	playlists->clear();
	library->clear();
	shows->clear();

	// Get current search text
	const QString currentSearchText = searchBox->text();

	// Save last searched query
	searchText = currentSearchText;

	// Search in library cache until tab is selected
	library->searchCache(searchText.toStdString());

	// Don't actually search if nothing to search on
	if (searchText.isEmpty())
	{
		return;
	}

	// Set placeholder to show searching status
	const QString originalPlaceholder = searchBox->placeholderText();
	searchBox->setPlaceholderText("Searching...");

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
				spotify.playlist(id, [this](const lib::result<lib::spt::playlist> &result)
				{
					if (!result.success())
					{
						StatusMessage::error(QString("Failed to find playlist: %1")
							.arg(QString::fromStdString(result.message())));

						return;
					}

					this->playlists->add(result.value());
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
			searchBox->setPlaceholderText(QString()); // Restore normal placeholder
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

void Search::View::resultsLoaded(const lib::spt::search_results &results)
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

	// Search done - restore normal placeholder
	searchBox->setPlaceholderText(QString());
}

void Search::View::onIndexChanged(int index)
{
	if (static_cast<SearchTab>(index) == SearchTab::Library)
	{
		library->search(searchText.toStdString());
	}
}

void Search::View::onSearchTextChanged()
{
	// Stop any pending search
	searchTimer->stop();
	
	// Get current text
	const QString currentText = searchBox->text();
	
	// If text is empty, clear results immediately
	if (currentText.isEmpty())
	{
		tracks->clear();
		artists->clear();
		albums->clear();
		playlists->clear();
		library->clear();
		shows->clear();
		searchText.clear();
		return;
	}
	
	// Only start timer for searches with at least 2 characters
	// This prevents excessive API calls for single characters
	if (currentText.length() >= 2)
	{
		searchTimer->start();
	}
}
