#include "view/sidepanel/view.hpp"

SidePanel::View::View(lib::spt::api &spotify, const lib::settings &settings,
	lib::cache &cache, const lib::http_client &httpClient, QWidget *parent)
	: QDockWidget(parent),
	spotify(spotify),
	settings(settings),
	cache(cache),
	httpClient(httpClient)
{
	title = new SidePanel::Title(this);
	setTitleBarWidget(title);

	stack = new QStackedWidget(this);
	setWidget(stack);

	setVisible(false);

	QTabBar::connect(title, &QTabBar::tabCloseRequested,
		this, &SidePanel::View::removeTab);

	QTabBar::connect(title, &QTabBar::currentChanged,
		this, &SidePanel::View::setCurrentIndex);

	QTabBar::connect(title, &QTabBar::tabMoved,
		this, &SidePanel::View::onTabMoved);
}

void SidePanel::View::openArtist(const std::string &artistId)
{
	auto *view = new Artist::View(spotify, artistId, cache, httpClient, this);
	addTab(view, "view-media-artist", "...",
		SidePanelType::Artist, QString::fromStdString(artistId));
}

void SidePanel::View::openAudioFeatures(const lib::spt::track &track)
{
	auto *view = new ::View::AudioFeatures(spotify, track.id, this);
	addTab(view, "view-statistics", QString::fromStdString(track.title()),
		SidePanelType::AudioFeatures, QString::fromStdString(track.id));
}

void SidePanel::View::openLyrics(const lib::spt::track &track)
{
	auto *view = new LyricsView(httpClient, cache, this);
	addTab(view, "view-media-lyrics", QString::fromStdString(track.title()),
		SidePanelType::Lyrics, QString::fromStdString(track.id));
	view->open(track);
}

void SidePanel::View::openSearch()
{
	if (searchView == nullptr)
	{
		searchView = new Search::View(spotify, cache, httpClient, this);
	}

	if (stack->indexOf(searchView) < 0)
	{
		addTab(searchView, "edit-find", "Search",
			SidePanelType::Search, QString());
	}

	setCurrentWidget(searchView);
	setVisible(true);
}

void SidePanel::View::closeSearch()
{
	removeTab(stack->indexOf(searchView));
}

auto SidePanel::View::findTab(SidePanelType type, const QString &name) -> QWidget *
{
	switch (type)
	{
		case SidePanelType::None:
			return find<QWidget *>(name);

		case SidePanelType::Artist:
			return find<Artist::View *>(name);

		case SidePanelType::Search:
			return find<Search::View *>(name);

		case SidePanelType::AudioFeatures:
			return find<::View::AudioFeatures *>(name);

		case SidePanelType::Lyrics:
			return find<LyricsView *>(name);

		default:
			return nullptr;
	}
}

void SidePanel::View::addTab(QWidget *widget, const QString &icon,
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

void SidePanel::View::removeTab(int index)
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

void SidePanel::View::setCurrentIndex(int index)
{
	title->setCurrentIndex(index);
	stack->setCurrentIndex(index);
}

void SidePanel::View::setCurrentWidget(QWidget *widget)
{
	auto index = stack->indexOf(widget);
	if (index >= 0)
	{
		setCurrentIndex(index);
	}
}

void SidePanel::View::setTabText(QWidget *widget, const QString &text)
{
	auto index = stack->indexOf(widget);
	if (index < 0)
	{
		return;
	}
	title->setTabText(index, text);
}

void SidePanel::View::onTabMoved(int from, int to)
{
	auto *fromWidget = stack->widget(from);
	if (fromWidget == nullptr)
	{
		return;
	}

	stack->removeWidget(fromWidget);
	stack->insertWidget(to, fromWidget);
}
