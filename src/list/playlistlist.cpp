#include "playlistlist.hpp"

#include "../mainwindow.hpp"

PlaylistList::PlaylistList(spt::Spotify &spotify, lib::settings &settings, lib::cache &cache,
	QWidget *parent)
	: spotify(spotify),
	settings(settings),
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
	auto *mainWindow = MainWindow::find(parentWidget());
	if (item != nullptr)
	{
		mainWindow->setCurrentLibraryItem(nullptr);
	}

	const auto &currentPlaylist = mainWindow->getPlaylist(getItemIndex(item));
	mainWindow->getSongsTree()->load(currentPlaylist);
}

void PlaylistList::doubleClicked(QListWidgetItem *item)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	const auto &currentPlaylist = mainWindow->getPlaylist(getItemIndex(item));
	mainWindow->getSongsTree()->load(currentPlaylist);

	spotify.play_tracks(lib::spt::api::to_uri("playlist", currentPlaylist.id),
		[mainWindow](const std::string &result)
		{
			if (result.empty())
			{
				return;
			}

			mainWindow->status(lib::fmt::format("Failed to start playlist playback: {}",
				result), true);
		});
}

void PlaylistList::menu(const QPoint &pos)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	const auto &playlist = mainWindow->getPlaylist(getItemIndex(itemAt(pos)));
	auto *menu = new PlaylistMenu(spotify, playlist, cache, mainWindow);
	menu->popup(mapToGlobal(pos));
}

auto PlaylistList::getPlaylists() -> std::vector<lib::spt::playlist> &
{
	return playlists;
}

void PlaylistList::load(const std::vector<lib::spt::playlist> &items)
{
	QListWidgetItem *activeItem = nullptr;
	const lib::spt::playlist *activePlaylist = nullptr;

	auto lastItem = currentItem() != nullptr
		? currentItem()->data(RolePlaylistId).toString().toStdString()
		: lib::spt::api::to_id(settings.general.last_playlist);

	playlists = items;

	// Add all playlists
	clear();
	auto i = 0;
	QTextDocument doc;
	for (const auto &playlist : playlists)
	{
		auto *item = new QListWidgetItem(QString::fromStdString(playlist.name), this);

		doc.setHtml(QString::fromStdString(playlist.description));
		item->setToolTip(doc.toPlainText());

		item->setData(RolePlaylistId, QString::fromStdString(playlist.id));
		item->setData(RoleIndex, i++);

		if (playlist.id == lastItem)
		{
			activeItem = item;
			activePlaylist = &playlist;
		}
	}

	// Sort
	if (settings.general.playlist_order != lib::playlist_order_default)
	{
		order(settings.general.playlist_order);
	}

	if (activeItem == nullptr || activePlaylist == nullptr)
	{
		if (count() > 0)
		{
			activeItem = item(0);
		}
		if (!playlists.empty())
		{
			activePlaylist = &playlists.at(0);
		}
	}

	if (currentItem() == nullptr && activePlaylist != nullptr)
	{
		auto *mainWindow = MainWindow::find(parentWidget());
		mainWindow->getSongsTree()->load(*activePlaylist);
	}

	if (activeItem != nullptr)
	{
		setCurrentItem(activeItem);
	}
}

void PlaylistList::refresh()
{
	spotify.playlists([this](const std::vector<lib::spt::playlist> &items)
	{
		load(items);
		cache.set_playlists(items);
	});
}

void PlaylistList::order(lib::playlist_order order)
{
	QList<QListWidgetItem *> items;
	items.reserve(count());

	auto i = 0;
	while (item(0) != nullptr)
	{
		items.insert(i, takeItem(0));
	}

	QMap<QString, int> customOrder;
	MainWindow *mainWindow;

	switch (order)
	{
		case lib::playlist_order_default:
			std::sort(items.begin(), items.end(), []
				(QListWidgetItem *i1, QListWidgetItem *i2) -> bool
			{
				return i1->data(RoleIndex).toInt() < i2->data(RoleIndex).toInt();
			});
			break;

		case lib::playlist_order_alphabetical:
			std::sort(items.begin(), items.end(), []
				(QListWidgetItem *i1, QListWidgetItem *i2) -> bool
			{
				return i1->text() < i2->text();
			});
			break;

		case lib::playlist_order_recent:
			// TODO: Currently sorts by when tracks where added, not when playlist was last played
			mainWindow = MainWindow::find(parentWidget());
			if (mainWindow == nullptr)
			{
				lib::log::error("Failed to order playlist: MainWindow not found");
				break;
			}

			std::sort(items.begin(), items.end(), [this]
				(QListWidgetItem *i1, QListWidgetItem *i2) -> bool
			{
				auto id1 = i1->data(DataRole::RolePlaylistId).toString().toStdString();
				auto id2 = i2->data(DataRole::RolePlaylistId).toString().toStdString();

				auto t1 = this->cache.get_playlist(id1).tracks;
				auto t2 = this->cache.get_playlist(id2).tracks;

				return !t1.empty() && !t2.empty()
					&& DateUtils::fromIso(t1.at(latestTrack(t1)).added_at)
						> DateUtils::fromIso(t2.at(latestTrack(t2)).added_at);
			});
			break;

		case lib::playlist_order_custom:
			i = 0;
			for (auto &playlist : settings.general.custom_playlist_order)
			{
				customOrder[QString::fromStdString(playlist)] = i++;
			}
			std::sort(items.begin(), items.end(), [customOrder]
				(QListWidgetItem *i1, QListWidgetItem *i2) -> bool
			{
				auto id1 = i1->data(DataRole::RolePlaylistId).toString();
				auto id2 = i2->data(DataRole::RolePlaylistId).toString();

				return customOrder.contains(id1) && customOrder.contains(id2)
					&& customOrder[id1] < customOrder[id2];
			});
			break;
	}

	for (auto *item : items)
	{
		addItem(item);
	}
}

auto PlaylistList::latestTrack(const std::vector<lib::spt::track> &tracks) -> int
{
	auto latest = 0;
	for (int i = 0; i < tracks.size(); i++)
	{
		if (DateUtils::fromIso(tracks[i].added_at)
			> DateUtils::fromIso(tracks[latest].added_at))
		{
			latest = i;
		}
	}
	return latest;
}
