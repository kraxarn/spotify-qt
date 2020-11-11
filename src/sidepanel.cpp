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
	addTab(artistView, Icon::get("view-media-artist"), artistView->windowTitle());
	setVisible(true);
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
	addTab(searchView, Icon::get("edit-find"), "Search");
	setVisible(true);
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
	addTab(audioFeatures, Icon::get("view-media-track"), QString("%1 - %2")
		.arg(artist)
		.arg(name));
	setVisible(true);
}
