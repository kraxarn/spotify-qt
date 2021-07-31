#include "view/search/playlists.hpp"
#include "mainwindow.hpp"

View::Search::Playlists::Playlists(lib::spt::api &spotify, lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	QListWidget(parent)
{
	QListWidget::connect(this, &QListWidget::itemClicked,
		this, &View::Search::Playlists::onItemClicked);

	setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &View::Search::Playlists::onContextMenu);
}

void View::Search::Playlists::add(const lib::spt::playlist &playlist)
{
	auto playlistName = QString::fromStdString(playlist.name);
	auto playlistId = QString::fromStdString(playlist.id);

	auto *item = new QListWidgetItem(playlistName, this);
	item->setData(RolePlaylistId, playlistId);
	item->setToolTip(playlistName);
}

void View::Search::Playlists::onItemClicked(QListWidgetItem *item)
{
	auto playlistId = item->data(RolePlaylistId).toString().toStdString();
	spotify.playlist(playlistId, [this](const lib::spt::playlist &playlist)
	{
		auto *mainWindow = MainWindow::find(this->parentWidget());
		mainWindow->getSongsTree()->load(playlist);
		mainWindow->setCurrentPlaylistItem(-1);
	});
}

void View::Search::Playlists::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	auto playlistId = item->data(RolePlaylistId).toString().toStdString();

	spotify.playlist(playlistId, [this, pos](const lib::spt::playlist &playlist)
	{
		auto *menu = new Menu::Playlist(spotify, playlist, cache, parentWidget());
		menu->popup(mapToGlobal(pos));
	});
}
