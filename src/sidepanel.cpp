#include "sidepanel.hpp"

SidePanel::SidePanel(spt::Spotify &spotify, const lib::settings &settings, lib::cache &cache,
	QWidget *parent)
	: spotify(spotify),
	settings(settings),
	parent(parent),
	cache(cache),
	QTabWidget(parent)
{
	setMovable(true);
	setTabsClosable(true);
	setVisible(false);
	setFixedWidth(320);

	QTabWidget::connect(this, &QTabWidget::tabCloseRequested,
		this, &SidePanel::removeTab);
}

void SidePanel::openArtist(const std::string &artistId)
{
	auto *view = new ArtistView(spotify, artistId, cache, parent);
	view->onArtistLoaded = [this, view](const lib::spt::artist &artist)
	{
		auto index = indexOf(view);
		if (index < 0)
			return;
		setTabText(index, QString::fromStdString(artist.name));
	};
	addAndSelect(view, "view-media-artist", "...");
}

void SidePanel::tabRemoved(int index)
{
	QTabWidget::tabRemoved(index);
	if (count() <= 0)
		setVisible(false);
}

void SidePanel::openSearch()
{
	if (searchView == nullptr)
		searchView = new SearchView(spotify, settings, cache, parent);
	addAndSelect(searchView, "edit-find", "Search");
}

void SidePanel::closeSearch()
{
	removeTab(indexOf(searchView));
}

void SidePanel::openAudioFeatures(const std::string &trackId,
	const std::string &artist, const std::string &name)
{
	auto view = new AudioFeaturesView(spotify, trackId, this);
	addAndSelect(view, "view-statistics", QString("%1 - %2")
		.arg(QString::fromStdString(artist))
		.arg(QString::fromStdString(name)));
}

void SidePanel::addAndSelect(QWidget *widget, const QString &icon, const QString &title)
{
	setCurrentIndex(addTab(widget, Icon::get(icon), title));
	setVisible(true);
}
