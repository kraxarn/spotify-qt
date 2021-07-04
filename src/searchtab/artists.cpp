#include "searchtab/artists.hpp"
#include "mainwindow.hpp"

SearchTab::Artists::Artists(QWidget *parent)
	: QListWidget(parent)
{
	QListWidget::connect(this, &QListWidget::itemClicked,
		this, &SearchTab::Artists::onItemClicked);
}

void SearchTab::Artists::add(const lib::spt::artist &artist)
{
	auto name = QString::fromStdString(artist.name);
	auto id = QString::fromStdString(artist.id);

	auto *item = new QListWidgetItem(name, this);
	item->setData(RoleArtistId, id);
	item->setToolTip(name);
}

void SearchTab::Artists::onItemClicked(QListWidgetItem *item)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->openArtist(item->data(RoleArtistId).toString().toStdString());
	parentWidget()->close();
}
