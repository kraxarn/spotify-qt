#include "view/search/artists.hpp"
#include "mainwindow.hpp"

View::Search::Artists::Artists(QWidget *parent)
	: QListWidget(parent)
{
	QListWidget::connect(this, &QListWidget::itemClicked,
		this, &View::Search::Artists::onItemClicked);
}

void View::Search::Artists::add(const lib::spt::artist &artist)
{
	auto name = QString::fromStdString(artist.name);
	auto id = QString::fromStdString(artist.id);

	auto *item = new QListWidgetItem(name, this);
	item->setData(RoleArtistId, id);
	item->setToolTip(name);
}

void View::Search::Artists::onItemClicked(QListWidgetItem *item)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->openArtist(item->data(RoleArtistId).toString().toStdString());
}
