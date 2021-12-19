#include "view/artist/sharemenu.hpp"
#include "mainwindow.hpp"

Artist::ShareMenu::ShareMenu(const lib::spt::artist &artist, QWidget *parent)
	: QMenu(parent),
	artist(artist)
{
	setIcon(Icon::get("document-share"));
	setTitle("Share");

	auto *copyArtistLink = addAction("Copy artist link");
	QAction::connect(copyArtistLink, &QAction::triggered,
		this, &Artist::ShareMenu::onCopyArtistLink);

	auto *openInSpotify = addAction("Open in Spotify");
	QAction::connect(openInSpotify, &QAction::triggered,
		this, &Artist::ShareMenu::onOpenInSpotify);
}

void Artist::ShareMenu::onCopyArtistLink(bool /*checked*/)
{
	const auto artistLink = QString("https://open.spotify.com/artist/%1")
		.arg(QString::fromStdString(artist.id));

	QApplication::clipboard()->setText(artistLink);
	StatusMessage::info(QStringLiteral("Link copied to clipboard"));
}

void Artist::ShareMenu::onOpenInSpotify(bool /*checked*/)
{
	const auto url = lib::fmt::format("https://open.spotify.com/artist/{}", artist.id);
	UrlUtils::open(url, LinkType::Web, MainWindow::find(parentWidget()));
}
