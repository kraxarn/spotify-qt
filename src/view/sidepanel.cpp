#include "sidepanel.hpp"

SidePanel::SidePanel(spt::Spotify &spotify, const lib::settings &settings, lib::cache &cache,
	const lib::http_client &httpClient, QWidget *parent)
	: spotify(spotify),
	settings(settings),
	parent(parent),
	cache(cache),
	httpClient(httpClient),
	QDockWidget(parent)
{
	title = new View::SidePanelTitle(this);
	setTitleBarWidget(title);

	stack = new QStackedWidget(this);
	setWidget(stack);

	setVisible(false);

	QTabBar::connect(title, &QTabBar::tabCloseRequested,
		this, &SidePanel::removeTab);

	QTabBar::connect(title, &QTabBar::currentChanged,
		this, &SidePanel::setCurrentIndex);
}

void SidePanel::addTab(QWidget *widget, const QString &icon, const QString &tabTitle)
{
	auto index = stack->addWidget(widget);
	title->insertTab(index, Icon::get(icon), tabTitle);

	setCurrentIndex(index);
	setVisible(true);
}

void SidePanel::removeTab(int index)
{
	title->removeTab(index);
	stack->removeWidget(stack->widget(index));

	if (title->count() <= 0)
	{
		setVisible(false);
	}
}

void SidePanel::setCurrentIndex(int index)
{
	title->setCurrentIndex(index);
	stack->setCurrentIndex(index);
}

void SidePanel::setCurrentWidget(QWidget *widget)
{
	auto index = stack->indexOf(widget);
	if (index >= 0)
	{
		setCurrentIndex(index);
	}
}

void SidePanel::openArtist(const std::string &artistId)
{
	auto *view = new ArtistView(spotify, artistId, cache, httpClient, parent);
	view->onArtistLoaded = [this, view](const lib::spt::artist &artist)
	{
		auto index = stack->indexOf(view);
		if (index < 0)
		{
			return;
		}
		title->setTabText(index, QString::fromStdString(artist.name));
	};
	addTab(view, "view-media-artist", "...");
}

void SidePanel::openSearch()
{
	if (searchView == nullptr)
	{
		searchView = new SearchView(spotify, cache, httpClient, parent);
	}

	if (stack->indexOf(searchView) < 0)
	{
		addTab(searchView, "edit-find", "Search");
	}

	setCurrentWidget(searchView);
	setVisible(true);
}

void SidePanel::closeSearch()
{
	removeTab(stack->indexOf(searchView));
}

void SidePanel::openAudioFeatures(const lib::spt::track &track)
{
	auto *view = new AudioFeaturesView(spotify, track.id, this);
	addTab(view, "view-statistics",
		QString::fromStdString(track.title()));
}

void SidePanel::openLyrics(const lib::spt::track &track)
{
	auto *view = new LyricsView(httpClient, cache, this);
	addTab(view, "view-media-lyrics",
		QString::fromStdString(track.title()));
	view->open(track);
}