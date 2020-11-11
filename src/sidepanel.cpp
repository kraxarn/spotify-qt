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