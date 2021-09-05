#include "playlistlist.hpp"
#include "mainwindow.hpp"

PlaylistList::PlaylistList(lib::spt::api &spotify, lib::settings &settings, lib::cache &cache,
	QWidget *parent)
	: QListWidget(parent),
	spotify(spotify),
	cache(cache),
	settings(settings)
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
	if (!cached.empty())
	{
		load(cached);
	}

	refresh();
}

auto PlaylistList::getItemIndex(QListWidgetItem *item) -> int
{
	return item == nullptr
		? currentRow()
		: item->data(static_cast<int>(DataRole::Index)).toInt();
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

void PlaylistList::menu(const QPoint &pos)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	const auto &playlist = mainWindow->getPlaylist(getItemIndex(itemAt(pos)));
	auto *menu = new Menu::Playlist(spotify, playlist, cache, this);
	menu->popup(mapToGlobal(pos));
}

void PlaylistList::load(const std::vector<lib::spt::playlist> &playlists)
{
	QListWidgetItem *activeItem = nullptr;
	const lib::spt::playlist *activePlaylist = nullptr;

	auto lastItem = currentItem() != nullptr
		? currentItem()->data(static_cast<int>(DataRole::PlaylistId)).toString().toStdString()
		: lib::spt::api::to_id(settings.general.last_playlist);

	// Add all playlists
	clear();
	auto i = 0;
	QTextDocument doc;
	for (const auto &playlist: playlists)
	{
		auto *item = new QListWidgetItem(QString::fromStdString(playlist.name), this);

		doc.setHtml(QString::fromStdString(playlist.description));
		item->setToolTip(doc.toPlainText());

		item->setData(static_cast<int>(DataRole::PlaylistId), QString::fromStdString(playlist.id));
		item->setData(static_cast<int>(DataRole::DefaultIndex), i);
		item->setData(static_cast<int>(DataRole::Index), i++);

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

	if (order == lib::playlist_order::custom
		&& settings.general.custom_playlist_order.empty())
	{
		order = lib::playlist_order::none;
	}

	QMap<QString, int> customOrder;
	MainWindow *mainWindow;

	switch (order)
	{
		case lib::playlist_order::none:
			std::sort(items.begin(), items.end(),
				[](QListWidgetItem *i1, QListWidgetItem *i2) -> bool
				{
					return i1->data(static_cast<int>(DataRole::DefaultIndex)).toInt()
						< i2->data(static_cast<int>(DataRole::DefaultIndex)).toInt();
				});
			break;

		case lib::playlist_order::alphabetical:
			std::sort(items.begin(), items.end(),
				[](QListWidgetItem *i1, QListWidgetItem *i2) -> bool
				{
					return i1->text() < i2->text();
				});
			break;

		case lib::playlist_order::recent:
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
				auto id1 = i1->data(static_cast<int>(DataRole::PlaylistId))
					.toString().toStdString();
				auto id2 = i2->data(static_cast<int>(DataRole::PlaylistId))
					.toString().toStdString();

				auto t1 = this->cache.get_playlist(id1).tracks;
				auto t2 = this->cache.get_playlist(id2).tracks;

				return !t1.empty() && !t2.empty()
					&& DateUtils::fromIso(t1.at(latestTrack(t1)).added_at)
						> DateUtils::fromIso(t2.at(latestTrack(t2)).added_at);
			});
			break;

		case lib::playlist_order::custom:
			i = 0;
			for (auto &playlist: settings.general.custom_playlist_order)
			{
				customOrder[QString::fromStdString(playlist)] = i++;
			}
			std::sort(items.begin(), items.end(),
				[customOrder](QListWidgetItem *i1, QListWidgetItem *i2) -> bool
				{
					auto id1 = i1->data(static_cast<int>(DataRole::PlaylistId)).toString();
					auto id2 = i2->data(static_cast<int>(DataRole::PlaylistId)).toString();

					return customOrder.contains(id1)
						&& customOrder.contains(id2)
						&& customOrder[id1] < customOrder[id2];
				});
			break;
	}

	i = 0;
	for (auto *item: items)
	{
		item->setData(static_cast<int>(DataRole::Index), i++);
		addItem(item);
	}
}

auto PlaylistList::latestTrack(const std::vector<lib::spt::track> &tracks) -> int
{
	size_t latest = 0;
	for (size_t i = 0; i < tracks.size(); i++)
	{
		if (DateUtils::fromIso(tracks[i].added_at)
			> DateUtils::fromIso(tracks[latest].added_at))
		{
			latest = i;
		}
	}
	return latest;
}

auto PlaylistList::allArtists() -> std::unordered_set<std::string>
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

auto PlaylistList::at(int index) -> lib::spt::playlist
{
	auto *i = item(index);
	if (i == nullptr)
	{
		return {};
	}

	return at(i->data(static_cast<int>(DataRole::PlaylistId)).toString().toStdString());
}

auto PlaylistList::at(const std::string &id) -> lib::spt::playlist
{
	for (const auto &playlist: cache.get_playlists())
	{
		if (lib::strings::ends_with(id, playlist.id))
		{
			return playlist;
		}
	}

	return {};
}
