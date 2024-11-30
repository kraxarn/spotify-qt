#include "view/sidepanel/view.hpp"
#include "mainwindow.hpp"

SidePanel::View::View(lib::spt::api &spotify, lib::settings &settings,
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

	QTabBar::connect(title, &QTabBar::tabCloseRequested,
		this, &SidePanel::View::removeTab);

	QTabBar::connect(title, &QTabBar::currentChanged,
		this, &SidePanel::View::setCurrentIndex);

	QTabBar::connect(title, &QTabBar::tabMoved,
		this, &SidePanel::View::onTabMoved);
}

void SidePanel::View::openArtist(const std::string &artistId)
{
	auto *view = new Artist::View(spotify, artistId, cache, httpClient, settings, this);
	addTab(view, "view-media-artist", "...",
		SidePanelType::Artist, QString::fromStdString(artistId));
}

void SidePanel::View::openLyrics(const lib::spt::track &track)
{
	auto *view = new ::View::Lyrics(httpClient, cache, this);
	addTab(view, "view-media-lyrics", QString::fromStdString(track.title()),
		SidePanelType::Lyrics, QString::fromStdString(track.id));
	view->open(track);
}

void SidePanel::View::openLyrics(int lyricsId)
{
	auto *view = new ::View::Lyrics(httpClient, cache, this);
	const auto name = QString::number(lyricsId);
	addTab(view, QStringLiteral("view-media-lyrics"), name, SidePanelType::Lyrics, name);
	view->load(lyricsId);
}

void SidePanel::View::openSearch()
{
	if (searchView == nullptr)
	{
		searchView = new Search::View(spotify, cache, httpClient, settings, this);
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
		case SidePanelType::Artist:
			return find<Artist::View *>(name);

		case SidePanelType::Search:
			return nullptr;

		case SidePanelType::Lyrics:
			return find<::View::Lyrics *>(name);

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

	if (widget != searchView)
	{
		widget->deleteLater();
	}

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

	const auto tabText = QString(text).replace('&', QStringLiteral("&&"));
	title->setTabText(index, tabText);
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

auto SidePanel::View::find(QWidget *widget) -> View *
{
	auto *window = MainWindow::find(widget);
	if (window == nullptr)
	{
		return nullptr;
	}

	return window->findChild<SidePanel::View *>(QString(),
		Qt::FindDirectChildrenOnly);
}

void SidePanel::View::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);

	// When using QGraphicsEffect (like with now playing)
	// the dock title disappears for some reason, so we
	// force it to redraw to work around it
	if (title->height() == 0)
	{
		lib::log::debug("Title is hidden, forcing re-add");
		title->hide();
		title->show();
	}
}
