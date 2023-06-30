#include "list/playlist.hpp"
#include "mainwindow.hpp"

List::Playlist::Playlist(lib::spt::api &spotify, lib::settings &settings,
	lib::cache &cache, QWidget *parent)
	: QListWidget(parent),
	spotify(spotify),
	cache(cache),
	settings(settings)
{
	// Set default selected playlist
	setCurrentRow(0);

	QListWidget::connect(this, &QListWidget::itemClicked,
		this, &List::Playlist::onItemClicked);
	QListWidget::connect(this, &QListWidget::itemDoubleClicked,
		this, &List::Playlist::onItemDoubleClicked);

	setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(this, &QWidget::customContextMenuRequested,
		this, &List::Playlist::onContextMenuRequested);
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
		load(cached);
	}

	refresh();
}

auto List::Playlist::getItemIndex(QListWidgetItem *item) -> int
{
	return item == nullptr
		? currentRow()
		: item->data(static_cast<int>(DataRole::Index)).toInt();
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

			StatusMessage::error(QString("Failed to start playlist playback: %1")
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

void List::Playlist::load(const std::vector<lib::spt::playlist> &playlists)
{
	QListWidgetItem *activeItem = nullptr;
	const lib::spt::playlist *activePlaylist = nullptr;

	auto lastItem = currentItem() != nullptr
		? currentItem()->data(static_cast<int>(DataRole::PlaylistId)).toString().toStdString()
		: lib::spt::uri_to_id(settings.general.last_playlist);

	// Add all playlists
	clear();
	auto index = 0;
	QTextDocument doc;
	for (const auto &playlist: playlists)
	{
		auto *item = new QListWidgetItem(QString::fromStdString(playlist.name), this);

		doc.setHtml(QString::fromStdString(playlist.description));
		item->setToolTip(doc.toPlainText());

		item->setData(static_cast<int>(DataRole::PlaylistId), QString::fromStdString(playlist.id));
		item->setData(static_cast<int>(DataRole::DefaultIndex), index);
		item->setData(static_cast<int>(DataRole::Index), index++);

		if (playlist.id == lastItem)
		{
			activeItem = item;
			activePlaylist = &playlist;
		}
	}

	// Sort
	if (settings.general.playlist_order != lib::playlist_order::none)
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
			activePlaylist = playlists.data();
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

void List::Playlist::refresh()
{
	spotify.playlists([this](const std::vector<lib::spt::playlist> &items)
	{
		load(items);
		cache.set_playlists(items);
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

	QMap<QString, int> customOrder;
	QHash<QString, QDateTime> edited;
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
				const auto playlistIdData = item->data(static_cast<int>(DataRole::PlaylistId));
				const auto playlistId = playlistIdData.toString();
				// TODO: Getting playlist from cache (again) can still be slow
				const auto playlist = cache.get_playlist(playlistId.toStdString());
				edited[playlistId] = latestTrack(playlist.tracks);
			}

			std::sort(items.begin(), items.end(), [&edited](QListWidgetItem *item1, QListWidgetItem *item2) -> bool
			{
				const auto id1 = item1->data(static_cast<int>(DataRole::PlaylistId)).toString();
				const auto id2 = item2->data(static_cast<int>(DataRole::PlaylistId)).toString();

				const auto date1 = edited.value(id1);
				const auto date2 = edited.value(id2);

				return date1.isValid() && date2.isValid() && date1 > date2;
			});
			break;

		case lib::playlist_order::custom:
			auto index = 0;
			for (auto &playlist: settings.general.custom_playlist_order)
			{
				customOrder[QString::fromStdString(playlist)] = index++;
			}
			std::sort(items.begin(), items.end(),
				[customOrder](QListWidgetItem *item1, QListWidgetItem *item2) -> bool
				{
					auto id1 = item1->data(static_cast<int>(DataRole::PlaylistId)).toString();
					auto id2 = item2->data(static_cast<int>(DataRole::PlaylistId)).toString();

					return customOrder.contains(id1)
						&& customOrder.contains(id2)
						&& customOrder[id1] < customOrder[id2];
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
		auto playlistId = item(i)->data(static_cast<int>(DataRole::PlaylistId))
			.toString().toStdString();

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

auto List::Playlist::at(int index) -> lib::spt::playlist
{
	auto *listItem = item(index);
	if (listItem == nullptr)
	{
		return {};
	}

	return at(listItem->data(static_cast<int>(DataRole::PlaylistId)).toString().toStdString());
}

auto List::Playlist::at(const std::string &playlistId) -> lib::spt::playlist
{
	for (const auto &playlist: cache.get_playlists())
	{
		if (lib::strings::ends_with(playlistId, playlist.id))
		{
			return playlist;
		}
	}

	return {};
}
