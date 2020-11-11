#include "sidepanel.hpp"

SidePanel::SidePanel(spt::Spotify &spotify, const Settings &settings, QWidget *parent)
	: spotify(spotify), settings(settings), parent(parent), QTabWidget(parent)
{
	setMovable(true);
	setTabsClosable(true);
	setTabPosition(QTabWidget::South);
	setVisible(false);
	setFixedWidth(320);

	QTabWidget::connect(this, &QTabWidget::tabCloseRequested, this, &SidePanel::removeTab);
}

void SidePanel::openArtist(const QString &artistId)
{
	auto view = new ArtistView(spotify, artistId, settings, parent);
	if (artistView != nullptr)
	{
		artistView->close();
		artistView->deleteLater();
	}
	artistView = view;
	addAndSelect(artistView, "view-media-artist", artistView->windowTitle());
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
	{
		searchView = new SearchView(spotify, settings, parent);
	}
	addAndSelect(searchView, "edit-find", "Search");
}

void SidePanel::closeSearch()
{
	removeTab(indexOf(searchView));
}

void SidePanel::openAudioFeatures(const QString &trackId, const QString &artist, const QString &name)
{
	auto view = new AudioFeaturesView(spotify, trackId, this);
	if (audioFeatures != nullptr)
	{
		audioFeatures->close();
		audioFeatures->deleteLater();
	}
	audioFeatures = view;
	addAndSelect(audioFeatures, "view-media-track", QString("%1 - %2")
		.arg(artist)
		.arg(name));
}

void SidePanel::addAndSelect(QWidget *widget, const QString &icon, const QString &title)
{
	setCurrentIndex(addTab(widget, Icon::get(icon), title));
	setVisible(true);
}
