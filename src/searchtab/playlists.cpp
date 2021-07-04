#include "searchtab/playlists.hpp"
#include "mainwindow.hpp"

SearchTab::Playlists::Playlists(lib::spt::api &spotify, lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	QListWidget(parent)
{
	QListWidget::connect(this, &QListWidget::itemClicked,
		this, &SearchTab::Playlists::onItemClicked);

	setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &SearchTab::Playlists::onContextMenu);
}

void SearchTab::Playlists::add(const lib::spt::playlist &playlist)
{
	auto playlistName = QString::fromStdString(playlist.name);
	auto playlistId = QString::fromStdString(playlist.id);

	auto *item = new QListWidgetItem(playlistName, this);
	item->setData(RolePlaylistId, playlistId);
	item->setToolTip(playlistName);
}

void SearchTab::Playlists::onItemClicked(QListWidgetItem *item)
{
	auto playlistId = item->data(RolePlaylistId).toString().toStdString();
	spotify.playlist(playlistId, [this](const lib::spt::playlist &playlist)
	{
		auto *mainWindow = MainWindow::find(this->parentWidget());
		mainWindow->getSongsTree()->load(playlist);
	});
}

void SearchTab::Playlists::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	auto playlistId = item->data(RolePlaylistId).toString().toStdString();

	spotify.playlist(playlistId, [this, pos](const lib::spt::playlist &playlist)
	{
		auto *menu = new PlaylistMenu(spotify, playlist, cache, parentWidget());
		menu->popup(mapToGlobal(pos));
	});
}
