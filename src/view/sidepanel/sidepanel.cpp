#include "view/sidepanel/sidepanel.hpp"

View::SidePanel::SidePanel::SidePanel(lib::spt::api &spotify, const lib::settings &settings,
	lib::cache &cache, const lib::http_client &httpClient, QWidget *parent)
	: QDockWidget(parent),
	parent(parent),
	spotify(spotify),
	settings(settings),
	cache(cache),
	httpClient(httpClient)
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

	QTabBar::connect(title, &QTabBar::tabMoved,
		this, &View::SidePanel::SidePanel::onTabMoved);
}

void View::SidePanel::SidePanel::openArtist(const std::string &artistId)
{
	auto *view = new View::Artist::Artist(spotify, artistId, cache, httpClient, parent);
	addTab(view, "view-media-artist", "...",
		SidePanelType::Artist, QString::fromStdString(artistId));
}

void View::SidePanel::SidePanel::openAudioFeatures(const lib::spt::track &track)
{
	auto *view = new View::AudioFeatures(spotify, track.id, this);
	addTab(view, "view-statistics", QString::fromStdString(track.title()),
		SidePanelType::AudioFeatures, QString::fromStdString(track.id));
}

void View::SidePanel::SidePanel::openLyrics(const lib::spt::track &track)
{
	auto *view = new LyricsView(httpClient, cache, this);
	addTab(view, "view-media-lyrics", QString::fromStdString(track.title()),
		SidePanelType::Lyrics, QString::fromStdString(track.id));
	view->open(track);
}

void View::SidePanel::SidePanel::openSearch()
{
	if (searchView == nullptr)
	{
		searchView = new View::Search::Search(spotify, cache, httpClient, parent);
	}

	if (stack->indexOf(searchView) < 0)
	{
		addTab(searchView, "edit-find", "Search", SidePanelType::Search, QString());
	}

	setCurrentWidget(searchView);
	setVisible(true);
}

void View::SidePanel::SidePanel::closeSearch()
{
	removeTab(stack->indexOf(searchView));
}

auto View::SidePanel::SidePanel::findTab(SidePanelType type, const QString &name) -> QWidget *
{
	switch (type)
	{
		case SidePanelType::None:
			return find<QWidget *>(name);

		case SidePanelType::Artist:
			return find<View::Artist::Artist *>(name);

		case SidePanelType::Search:
			return find<View::Search::Search *>(name);

		case SidePanelType::AudioFeatures:
			return find<View::AudioFeatures *>(name);

		case SidePanelType::Lyrics:
			return find<LyricsView *>(name);

		default:
			return nullptr;
	}
}

void View::SidePanel::SidePanel::addTab(QWidget *widget, const QString &icon,
	const QString &tabTitle, SidePanelType type, const QString &name)
{
	auto *current = findTab(type, name);
	if (current != nullptr)
	{
		setCurrentWidget(current);
		setVisible(true);
		return;
	}

	widget->setObjectName(name);
	auto index = stack->addWidget(widget);
	title->insertTab(index, Icon::get(icon), tabTitle);

	setCurrentIndex(index);
	setVisible(true);
}

void View::SidePanel::SidePanel::removeTab(int index)
{
	title->removeTab(index);

	auto *widget = stack->widget(index);
	stack->removeWidget(widget);
	widget->deleteLater();

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

void View::SidePanel::SidePanel::setTabText(QWidget *widget, const QString &text)
{
	auto index = stack->indexOf(widget);
	if (index < 0)
	{
		return;
	}
	title->setTabText(index, text);
}

void View::SidePanel::SidePanel::onTabMoved(int from, int to)
{
	auto *fromWidget = stack->widget(from);
	if (fromWidget == nullptr)
	{
		return;
	}

	stack->removeWidget(fromWidget);
	stack->insertWidget(to, fromWidget);
}
