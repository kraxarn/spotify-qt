#include "playlistlist.hpp"

PlaylistList::PlaylistList(spt::Spotify &spotify, QWidget *parent)
	: spotify(spotify), QListWidget(parent)
{
	this->parent = dynamic_cast<MainWindow *>(parent);
	if (this->parent == nullptr)
		return;

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
	auto mainWindow = (MainWindow *) parent;
	if (item != nullptr)
		mainWindow->getLibraryList()->setCurrentItem(nullptr);

	auto currentPlaylist = mainWindow->getSptPlaylists().at(getItemIndex(item));
	mainWindow->loadPlaylist(currentPlaylist);
}

void PlaylistList::doubleClicked(QListWidgetItem *item)
{
	auto mainWindow = (MainWindow *) parent;
	auto currentPlaylist = mainWindow->getSptPlaylists().at(getItemIndex(item));
	mainWindow->loadPlaylist(currentPlaylist);

	auto result = spotify.playTracks(
		QString("spotify:playlist:%1").arg(currentPlaylist.id));
	if (!result.isEmpty())
		mainWindow->setStatus(QString("Failed to start playlist playback: %1").arg(result), true);
}

void PlaylistList::menu(const QPoint &pos)
{
	auto mainWindow = (MainWindow *) parent;
	(new PlaylistMenu(spotify,
		mainWindow->getSptPlaylists().at(getItemIndex(itemAt(pos))), parent))
		->popup(mapToGlobal(pos));
}

