#include "view/sidepanel/sidepanel.hpp"

View::SidePanel::SidePanel::SidePanel(spt::Spotify &spotify, const lib::settings &settings,
	lib::cache &cache, const lib::http_client &httpClient, QWidget *parent)
	: spotify(spotify),
	settings(settings),
	parent(parent),
	cache(cache),
	httpClient(httpClient),
	QDockWidget(parent)
{
	title = new View::SidePanel::Title(this);
	setTitleBarWidget(title);

	stack = new QStackedWidget(this);
	setWidget(stack);

	setVisible(false);

	QTabBar::connect(title, &QTabBar::tabCloseRequested,
		this, &View::SidePanel::SidePanel::removeTab);

	QTabBar::connect(title, &QTabBar::currentChanged,
		this, &View::SidePanel::SidePanel::setCurrentIndex);
}

void View::SidePanel::SidePanel::addTab(QWidget *widget, const QString &icon,
	const QString &tabTitle)
{
	auto index = stack->addWidget(widget);
	title->insertTab(index, Icon::get(icon), tabTitle);

	setCurrentIndex(index);
	setVisible(true);
}

void View::SidePanel::SidePanel::removeTab(int index)
{
	title->removeTab(index);
	stack->removeWidget(stack->widget(index));

	if (title->count() <= 0)
	{
		setVisible(false);
	}
}

void View::SidePanel::SidePanel::setCurrentIndex(int index)
{
	title->setCurrentIndex(index);
	stack->setCurrentIndex(index);
}

void View::SidePanel::SidePanel::setCurrentWidget(QWidget *widget)
{
	auto index = stack->indexOf(widget);
	if (index >= 0)
	{
		setCurrentIndex(index);
	}
}

void View::SidePanel::SidePanel::openArtist(const std::string &artistId)
{
	auto *view = new View::Artist::Artist(spotify, artistId, cache, httpClient, parent);
	addTab(view, "view-media-artist", "...");
}

void View::SidePanel::SidePanel::setTabText(QWidget *widget, const QString &text)
{
	auto index = stack->indexOf(widget);
	if (index < 0)
	{
		return;
	}
	title->setTabText(index, text);
}

void View::SidePanel::SidePanel::openSearch()
{
	if (searchView == nullptr)
	{
		searchView = new View::Search::Search(spotify, cache, httpClient, parent);
	}

	if (stack->indexOf(searchView) < 0)
	{
		addTab(searchView, "edit-find", "Search");
	}

	setCurrentWidget(searchView);
	setVisible(true);
}

void View::SidePanel::SidePanel::closeSearch()
{
	removeTab(stack->indexOf(searchView));
}

void View::SidePanel::SidePanel::openAudioFeatures(const lib::spt::track &track)
{
	auto *view = new View::AudioFeatures(spotify, track.id, this);
	addTab(view, "view-statistics",
		QString::fromStdString(track.title()));
}

void View::SidePanel::SidePanel::openLyrics(const lib::spt::track &track)
{
	auto *view = new LyricsView(httpClient, cache, this);
	addTab(view, "view-media-lyrics",
		QString::fromStdString(track.title()));
	view->open(track);
}
