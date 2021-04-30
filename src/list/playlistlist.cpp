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

	QListWidget::connect(this, &QListWidget::itemClicked,
		this, &PlaylistList::clicked);
	QListWidget::connect(this, &QListWidget::itemDoubleClicked,
		this, &PlaylistList::doubleClicked);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &PlaylistList::menu);
}

void PlaylistList::showEvent(QShowEvent */*event*/)
{
	// Playlists are already loaded, don't load from cache
	if (count() > 0)
	{
		return;
	}

	const auto &cached = cache.get_playlists();
	if (cached.empty())
	{
		return;
	}

	load(cached);
}

auto PlaylistList::getItemIndex(QListWidgetItem *item) -> int
{
	auto *playlistItem = item == nullptr
		? nullptr
		: dynamic_cast<PlaylistListItem *>(item);

	return playlistItem == nullptr
		? currentRow()
		: playlistItem->getIndex();
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

	std::string lastItem;
	if (currentItem() != nullptr)
	{
		auto *playlistItem = dynamic_cast<PlaylistListItem *>(currentItem());
		if (playlistItem != nullptr)
		{
			lastItem = playlistItem->getData().id;
		}
	}
	else
	{
		lastItem = lib::spt::api::to_id(settings.general.last_playlist);
	}

	playlists = items;

	// Add all playlists
	clear();
	auto i = 0;
	QTextDocument doc;
	for (const auto &playlist : playlists)
	{
		auto *item = new PlaylistListItem(playlist, i++, this);

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
	QList<PlaylistListItem *> items;
	items.reserve(count());

	while (item(0) != nullptr)
	{
		auto *playlistItem = dynamic_cast<PlaylistListItem *>(takeItem(0));
		if (playlistItem != nullptr)
		{
			items.insert(0, playlistItem);
		}
	}

	std::unordered_map<std::string, int> customOrder;
	MainWindow *mainWindow;
	int i;

	switch (order)
	{
		case lib::playlist_order_default:
			std::sort(items.begin(), items.end(), []
				(PlaylistListItem *i1, PlaylistListItem *i2) -> bool
			{
				return i1->getIndex() < i2->getIndex();
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
				(PlaylistListItem *i1, PlaylistListItem *i2) -> bool
			{
				const auto &pl1 = i1->getData();
				const auto &pl2 = i2->getData();

				const auto &t1 = (pl1.tracks.empty()
					? this->cache.get_playlist(pl1.id)
					: pl1).tracks;
				const auto &t2 = (pl2.tracks.empty()
					? this->cache.get_playlist(pl2.id)
					: pl2).tracks;

				return !t1.empty() && !t2.empty()
					&& DateUtils::fromIso(t1.at(latestTrack(t1)).added_at)
						> DateUtils::fromIso(t2.at(latestTrack(t2)).added_at);
			});
			break;

		case lib::playlist_order_custom:
			i = 0;
			for (auto &playlist : settings.general.custom_playlist_order)
			{
				customOrder[playlist] = i++;
			}
			std::sort(items.begin(), items.end(), [customOrder]
				(PlaylistListItem *i1, PlaylistListItem *i2) -> bool
			{
				const auto &id1 = i1->getData().id;
				const auto &id2 = i2->getData().id;

				return lib::map::contains(customOrder, id1)
					&& lib::map::contains(customOrder, id2)
					&& customOrder.at(id1) < customOrder.at(id2);
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
