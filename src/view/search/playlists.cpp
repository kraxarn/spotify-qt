#include "view/search/playlists.hpp"
#include "mainwindow.hpp"

Search::Playlists::Playlists(lib::spt::api &spotify, lib::cache &cache, QWidget *parent)
	: QListWidget(parent),
	spotify(spotify),
	cache(cache)
{
	QListWidget::connect(this, &QListWidget::itemClicked,
		this, &Search::Playlists::onItemClicked);

	QListWidget::connect(this, &QListWidget::itemDoubleClicked,
		this, &Search::Playlists::onItemDoubleClicked);

	setContextMenuPolicy(Qt::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &Search::Playlists::onContextMenu);
}

void Search::Playlists::add(const lib::spt::playlist &playlist)
{
	auto playlistName = QString::fromStdString(playlist.name);
	auto playlistId = QString::fromStdString(playlist.id);

	auto *item = new QListWidgetItem(playlistName, this);
	item->setData(static_cast<int>(DataRole::Playlist), QVariant::fromValue(playlist));
	item->setToolTip(playlistName);
}

void Search::Playlists::onItemClicked(QListWidgetItem *item)
{
	const auto &playlistData = item->data(static_cast<int>(DataRole::Playlist));
	const auto playlist = playlistData.value<lib::spt::playlist>();

	spotify.playlist(playlist.id, [this](const lib::spt::playlist &playlist)
	{
		auto *mainWindow = MainWindow::find(this->parentWidget());
		mainWindow->getSongsTree()->load(playlist);
		mainWindow->setCurrentPlaylistItem(-1);
	});
}

void Search::Playlists::onItemDoubleClicked(QListWidgetItem *item)
{
	const auto &playlistData = item->data(static_cast<int>(DataRole::Playlist));
	const auto playlist = playlistData.value<lib::spt::playlist>();

	spotify.play_tracks(lib::spt::id_to_uri("playlist", playlist.id), [](const std::string &result)
	{
		if (!result.empty())
		{
			StatusMessage::error(QString("Failed to start playlist playback: %1")
				.arg(QString::fromStdString(result)));
		}
	});
}

void Search::Playlists::onContextMenu(const QPoint &pos)
{
	auto *item = itemAt(pos);
	const auto &playlistData = item->data(static_cast<int>(DataRole::Playlist));
	const auto playlist = playlistData.value<lib::spt::playlist>();

	auto *menu = new Menu::Playlist(spotify, playlist, cache, parentWidget());
	menu->popup(mapToGlobal(pos));
}
