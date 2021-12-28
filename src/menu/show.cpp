#include "menu/show.hpp"
#include "mainwindow.hpp"

Menu::Show::Show(lib::spt::api &spotify, const lib::spt::show &show, QWidget *parent)
	: QMenu(parent),
	show(show),
	spotify(spotify)
{
	auto *publisherAction = addAction(QString("By %1")
		.arg(QString::fromStdString(show.publisher)));
	publisherAction->setEnabled(false);

	addMenu(shareMenu());
}

auto Menu::Show::showUrl() const -> QString
{
	return QString("https://open.spotify.com/show/%1")
		.arg(QString::fromStdString(show.id));
}

auto Menu::Show::shareMenu() -> QMenu *
{
	auto *menu = new QMenu("Share", this);
	menu->setIcon(Icon::get("document-share"));

	auto *copyLink = menu->addAction("Copy podcast link");
	QAction::connect(copyLink, &QAction::triggered,
		this, &Menu::Show::onCopyLink);

	auto *openInSpotify = menu->addAction("Open in Spotify");
	QAction::connect(openInSpotify, &QAction::triggered,
		this, &Menu::Show::onOpenInSpotify);

	return menu;
}

void Menu::Show::onCopyLink(bool /*checked*/) const
{
	QApplication::clipboard()->setText(showUrl());
	StatusMessage::info(QStringLiteral("Link copied to clipboard"));
}

void Menu::Show::onOpenInSpotify(bool /*checked*/) const
{
	Url::open(showUrl(), LinkType::Web, MainWindow::find(parentWidget()));
}
