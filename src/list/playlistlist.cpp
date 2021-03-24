#include "playlistlist.hpp"

#include "../mainwindow.hpp"

PlaylistList::PlaylistList(spt::Spotify &spotify, lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	cache(cache),
	QListWidget(parent)
{
	// Set default selected playlist
	setCurrentRow(0);

	QListWidget::connect(this, &QListWidget::itemClicked, this, &PlaylistList::clicked);
	QListWidget::connect(this, &QListWidget::itemDoubleClicked, this, &PlaylistList::doubleClicked);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested, this, &PlaylistList::menu);
}

int PlaylistList::getItemIndex(QListWidgetItem *item)
{
	return item == nullptr
		? currentRow()
		: item->data(RoleIndex).toInt();
}

void PlaylistList::clicked(QListWidgetItem *item)
{
	auto mainWindow = MainWindow::find(parentWidget());
	if (item != nullptr)
		mainWindow->setCurrentLibraryItem(nullptr);

	auto currentPlaylist = mainWindow->getPlaylist(getItemIndex(item));
	mainWindow->getSongsTree()->load(currentPlaylist);
}

void PlaylistList::doubleClicked(QListWidgetItem *item)
{
	auto mainWindow = MainWindow::find(parentWidget());
	auto currentPlaylist = mainWindow->getPlaylist(getItemIndex(item));
	mainWindow->getSongsTree()->load(currentPlaylist);

	spotify.playTracks(QString("spotify:playlist:%1")
			.arg(QString::fromStdString(currentPlaylist.id)),
		[mainWindow](const QString &result)
		{
			if (!result.isEmpty())
			{
				mainWindow->setStatus(QString("Failed to start playlist playback: %1")
					.arg(result), true);
			}
		});
}

void PlaylistList::menu(const QPoint &pos)
{
	auto mainWindow = MainWindow::find(parentWidget());
	auto playlist = mainWindow->getPlaylist(getItemIndex(itemAt(pos)));
	auto menu = new PlaylistMenu(spotify, playlist, cache, mainWindow);
	menu->popup(mapToGlobal(pos));
}
