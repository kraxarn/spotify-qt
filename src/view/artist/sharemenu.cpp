#include "view/artist/sharemenu.hpp"
#include "mainwindow.hpp"

View::Artist::ShareMenu::ShareMenu(const lib::spt::artist &artist, QWidget *parent)
	: artist(artist),
	QMenu(parent)
{
	setIcon(Icon::get("document-share"));
	setTitle("Share");

	auto *copyArtistLink = addAction("Copy artist link");
	QAction::connect(copyArtistLink, &QAction::triggered,
		this, &View::Artist::ShareMenu::onCopyArtistLink);

	auto *openInSpotify = addAction("Open in Spotify");
	QAction::connect(openInSpotify, &QAction::triggered,
		this, &View::Artist::ShareMenu::onOpenInSpotify);
}

void View::Artist::ShareMenu::onCopyArtistLink(bool /*checked*/)
{
	const auto artistLink = QString("https://open.spotify.com/artist/%1")
		.arg(QString::fromStdString(artist.id));

	QApplication::clipboard()->setText(artistLink);
	MainWindow::find(parentWidget())->setStatus("Link copied to clipboard");
}

void View::Artist::ShareMenu::onOpenInSpotify(bool /*checked*/)
{
	const auto url = lib::fmt::format("https://open.spotify.com/artist/{}", artist.id);
	UrlUtils::open(url, LinkType::Web, MainWindow::find(parentWidget()));
}
