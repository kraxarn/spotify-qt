#include "list/playlist.hpp"
#include "mainwindow.hpp"

List::Playlist::Playlist(lib::spt::api &spotify, lib::settings &settings,
	lib::cache &cache, const lib::http_client &httpClient, QWidget *parent)
	: QListWidget(parent),
	spotify(spotify),
	cache(cache),
	settings(settings),
	tooltip(settings, httpClient, cache)
{
	// Set default selected playlist
	setCurrentRow(0);

	setMouseTracking(true);

	QListWidget::connect(this, &QListWidget::itemClicked,
		this, &List::Playlist::onItemClicked);
	QListWidget::connect(this, &QListWidget::itemDoubleClicked,
		this, &List::Playlist::onItemDoubleClicked);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &List::Playlist::onContextMenuRequested);

	QListWidget::connect(this, &QListWidget::itemEntered,
		this, &List::Playlist::onItemEntered);
}

void List::Playlist::showEvent(QShowEvent */*event*/)
{
	// Playlists are already loaded, don't load from cache
	if (count() > 0)
	{
		return;
	}

	const auto &cached = cache.get_playlists();
	if (!cached.empty())
	{
		load(cached, 0);
		selectActive();
	}

	refresh();
}

auto List::Playlist::getItemIndex(QListWidgetItem *item) -> int
{
	return item == nullptr
		? currentRow()
		: item->data(static_cast<int>(DataRole::Index)).toInt();
}

auto List::Playlist::getPlaylists() const -> std::vector<lib::spt::playlist>
{
	std::vector<lib::spt::playlist> result;
	result.reserve(static_cast<size_t>(count()));

	for (auto i = 0; i < count(); i++)
	{
		result.push_back(at(i));
	}

	return result;
}

void List::Playlist::onItemClicked(QListWidgetItem *item)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	if (item != nullptr)
	{
		mainWindow->setCurrentLibraryItem(nullptr);
	}

	const auto &currentPlaylist = mainWindow->getPlaylist(getItemIndex(item));
	mainWindow->getSongsTree()->load(currentPlaylist);
}

void List::Playlist::onItemDoubleClicked(QListWidgetItem *item)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	const auto &currentPlaylist = mainWindow->getPlaylist(getItemIndex(item));
	mainWindow->getSongsTree()->load(currentPlaylist);

	spotify.play_tracks(lib::spt::id_to_uri("playlist", currentPlaylist.id),
		[](const std::string &result)
		{
			if (result.empty())
			{
				return;
			}

			StatusMessage::error(QStringLiteral("Failed to start playlist playback: %1")
				.arg(QString::fromStdString(result)));
		});
}

void List::Playlist::onContextMenuRequested(const QPoint &pos)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	const auto &playlist = mainWindow->getPlaylist(getItemIndex(itemAt(pos)));
	auto *menu = new Menu::Playlist(spotify, playlist, cache, this);
	menu->popup(mapToGlobal(pos));
}

void List::Playlist::onItemEntered(QListWidgetItem *item)
{
	if (!item->toolTip().isEmpty())
	{
		return;
	}

	const auto &playlistData = item->data(static_cast<int>(DataRole::Playlist));
	const auto playlist = playlistData.value<lib::spt::playlist>();
	tooltip.set(item, playlist);
}

void List::Playlist::load(const std::vector<lib::spt::playlist> &playlists, const int offset)
{
	auto index = offset;

	for (const auto &playlist: playlists)
	{
		if (playlist.id.empty())
		{
			continue;
		}

		auto *item = new QListWidgetItem(QString::fromStdString(playlist.name), this);
		item->setData(static_cast<int>(DataRole::Playlist), QVariant::fromValue(playlist));
		item->setData(static_cast<int>(DataRole::DefaultIndex), index);
		item->setData(static_cast<int>(DataRole::Index), index++);
	}

	// Sort
	if (settings.general.playlist_order != lib::playlist_order::none)
	{
		order(settings.general.playlist_order);
	}
}

void List::Playlist::selectActive()
{
	QListWidgetItem *activeItem = nullptr;
	lib::spt::playlist activePlaylist;

	std::string currentPlaylistId;
	if (currentItem() != nullptr)
	{
		const auto &playlistData = currentItem()->data(static_cast<int>(DataRole::Playlist));
		const auto playlist = playlistData.value<lib::spt::playlist>();
		currentPlaylistId = playlist.id;
	}
	else
	{
		currentPlaylistId = lib::spt::uri_to_id(settings.general.last_playlist);
	}

	for (auto i = 0; i < count(); i++)
	{
		auto *listItem = item(i);
		const auto &playlist = at(i);

		if (playlist.id == currentPlaylistId)
		{
			activeItem = listItem;
			activePlaylist = playlist;
		}
	}

	if (activeItem == nullptr)
	{
		activeItem = item(0);
	}

	if (!activePlaylist.is_valid())
	{
		activePlaylist = at(0);
	}

	if (currentItem() == nullptr && activePlaylist.is_valid())
	{
		auto *mainWindow = MainWindow::find(parentWidget());
		mainWindow->getSongsTree()->load(activePlaylist);
	}

	if (activeItem != nullptr)
	{
		setCurrentItem(activeItem);
	}
}

void List::Playlist::refresh()
{
	spotify.playlists([this](const lib::result<lib::spt::page<lib::spt::playlist>> &result)
	{
		if (!result.success())
		{
			lib::log::error("Failed to get playlists: {}", result.message());
			return false;
		}

		const auto &page = result.value();
		if (page.offset == 0)
		{
			clear();
		}

		load(page.items, page.offset);
		if (page.has_next())
		{
			return true;
		}

		selectActive();
		cache.set_playlists(getPlaylists());
		return false;
	});
}

void List::Playlist::order(lib::playlist_order order)
{
	QList<QListWidgetItem *> items;
	items.reserve(count());

	QListWidgetItem *selectedItem = nullptr;
	while (count() > 0)
	{
		auto *current = item(0);
		if (selectedItem == nullptr && current->isSelected())
		{
			selectedItem = current;
		}
		items.append(takeItem(0));
	}

	if (order == lib::playlist_order::custom
		&& settings.general.custom_playlist_order.empty())
	{
		order = lib::playlist_order::none;
	}

	std::unordered_map<std::string, int> customOrder;
	std::unordered_map<std::string, QDateTime> edited;
	MainWindow *mainWindow;

	switch (order)
	{
		case lib::playlist_order::none:
			std::sort(items.begin(), items.end(),
				[](QListWidgetItem *item1, QListWidgetItem *item2) -> bool
				{
					return item1->data(static_cast<int>(DataRole::DefaultIndex)).toInt()
						< item2->data(static_cast<int>(DataRole::DefaultIndex)).toInt();
				});
			break;

		case lib::playlist_order::alphabetical:
			std::sort(items.begin(), items.end(),
				[](QListWidgetItem *item1, QListWidgetItem *item2) -> bool
				{
					return item1->text() < item2->text();
				});
			break;

		case lib::playlist_order::recent:
			mainWindow = MainWindow::find(parent());
			if (mainWindow == nullptr)
			{
				break;
			}

			for (const auto &item: items)
			{
				// We assume item data doesn't contain any tracks, so fetch from cache instead
				const auto &playlistData = item->data(static_cast<int>(DataRole::Playlist));
				const auto playlistId = playlistData.value<lib::spt::playlist>().id;
				// TODO: Getting playlist from cache (again) can still be slow
				const auto playlist = cache.get_playlist(playlistId);
				edited[playlistId] = latestTrack(playlist.tracks);
			}

			std::sort(items.begin(), items.end(), [&edited](QListWidgetItem *item1, QListWidgetItem *item2) -> bool
			{
				const auto &data1 = item1->data(static_cast<int>(DataRole::Playlist));
				const auto &data2 = item2->data(static_cast<int>(DataRole::Playlist));

				const auto id1 = data1.value<lib::spt::playlist>().id;
				const auto id2 = data2.value<lib::spt::playlist>().id;

				const auto iter1 = edited.find(id1);
				const auto iter2 = edited.find(id2);
				if (iter1 == edited.end() || iter2 == edited.end())
				{
					return false;
				}

				const auto &date1 = iter1->second;
				const auto &date2 = iter2->second;
				return date1.isValid() && date2.isValid() && date1 > date2;
			});
			break;

		case lib::playlist_order::custom:
			auto index = 0;
			for (auto &playlist: settings.general.custom_playlist_order)
			{
				customOrder[playlist] = index++;
			}

			std::sort(items.begin(), items.end(),
				[&customOrder](QListWidgetItem *item1, QListWidgetItem *item2) -> bool
				{
					const auto &data1 = item1->data(static_cast<int>(DataRole::Playlist));
					const auto &data2 = item2->data(static_cast<int>(DataRole::Playlist));

					const auto id1 = data1.value<lib::spt::playlist>().id;
					const auto id2 = data2.value<lib::spt::playlist>().id;

					const auto iter1 = customOrder.find(id1);
					const auto iter2 = customOrder.find(id2);

					return iter1 != customOrder.end()
						&& iter2 != customOrder.end()
						&& iter1->second < iter2->second;
				});
			break;
	}

	auto index = 0;
	for (auto *item: items)
	{
		item->setData(static_cast<int>(DataRole::Index), index++);
		addItem(item);
		if (item == selectedItem)
		{
			item->setSelected(true);
		}
	}
}

auto List::Playlist::latestTrack(const std::vector<lib::spt::track> &tracks) -> QDateTime
{
	QDateTime latest;
	for (const auto &track: tracks)
	{
		const auto addedAt = DateTime::parseIso(track.added_at);
		if (!latest.isValid() || addedAt > latest)
		{
			latest = addedAt;
		}
	}
	return latest;
}

auto List::Playlist::allArtists() -> std::unordered_set<std::string>
{
	std::unordered_set<std::string> artists;

	for (auto i = 0; i < count(); i++)
	{
		const auto &playlistData = item(i)->data(static_cast<int>(DataRole::Playlist));
		const auto playlistId = playlistData.value<lib::spt::playlist>().id;

		for (auto &track: cache.get_playlist(playlistId).tracks)
		{
			for (const auto &artist: track.artists)
			{
				artists.insert(artist.name);
			}
		}
	}

	return artists;
}

auto List::Playlist::at(const int index) const -> lib::spt::playlist
{
	const auto *listItem = item(index);
	if (listItem == nullptr)
	{
		return {};
	}

	const auto &playlistData = listItem->data(static_cast<int>(DataRole::Playlist));
	return playlistData.value<lib::spt::playlist>();
}
