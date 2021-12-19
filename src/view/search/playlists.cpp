#include "view/search/playlists.hpp"
#include "mainwindow.hpp"

Search::Playlists::Playlists(lib::spt::api &spotify, lib::cache &cache, QWidget *parent)
	: QListWidget(parent),
	spotify(spotify),
	cache(cache)
{
	QListWidget::connect(this, &QListWidget::itemClicked,
		this, &Search::Playlists::onItemClicked);

	setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &Search::Playlists::onContextMenu);
}

void Search::Playlists::add(const lib::spt::playlist &playlist)
{
	auto playlistName = QString::fromStdString(playlist.name);
	auto playlistId = QString::fromStdString(playlist.id);

	auto *item = new QListWidgetItem(playlistName, this);
	item->setData(static_cast<int>(DataRole::PlaylistId), playlistId);
	item->setToolTip(playlistName);
}

void Search::Playlists::onItemClicked(QListWidgetItem *item)
{
	auto playlistId = item->data(static_cast<int>(DataRole::PlaylistId))
		.toString().toStdString();
	spotify.playlist(playlistId, [this](const lib::spt::playlist &playlist)
	{
		auto *mainWindow = MainWindow::find(this->parentWidget());
		mainWindow->getSongsTree()->load(playlist);
		mainWindow->setCurrentPlaylistItem(-1);
	});
}

void Search::Playlists::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	auto playlistId = item->data(static_cast<int>(DataRole::PlaylistId))
		.toString().toStdString();

	spotify.playlist(playlistId, [this, pos](const lib::spt::playlist &playlist)
	{
		auto *menu = new Menu::Playlist(spotify, playlist, cache, parentWidget());
		menu->popup(mapToGlobal(pos));
	});
}
