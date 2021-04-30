#include "leftsidepanel.hpp"

#include "mainwindow.hpp"

LeftSidePanel::LeftSidePanel(spt::Spotify &spotify, lib::settings &settings,
	spt::Current &current, lib::cache &cache, QWidget *parent)
	: spotify(spotify),
	settings(settings),
	current(current),
	cache(cache),
	QWidget(parent)
{
	auto *layout = new QVBoxLayout(this);
	setMaximumWidth(250); // TODO: ?

	// Library
	libraryList = new LibraryList(spotify, parent);
	auto *library = Utils::createGroupBox(QVector<QWidget *>() << libraryList, parent);
	library->setTitle("Library");
	library->setWindowIcon(Icon::get("view-media-playlist"));
	layout->addWidget(library);

	// Playlists
	playlists = new PlaylistList(spotify, settings, cache, parent);
	refreshPlaylists();
	auto *playlistContainer = Utils::createGroupBox(QVector<QWidget *>() << playlists, parent);
	playlistContainer->setTitle("Playlists");
	layout->addWidget(playlistContainer);

	// Current context info
	auto *contextLayout = new QHBoxLayout();
	contextIcon = new QLabel(this);
	contextIcon->setVisible(false);
	contextInfo = new QLabel(this);
	contextInfo->setToolTip("Currently playing from");
	contextInfo->setVisible(false);
	contextLayout->addSpacing(16);
	contextLayout->addWidget(contextIcon);
	contextLayout->addWidget(contextInfo, 1);
	layout->addLayout(contextLayout);
	contextInfo->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QWidget::connect(contextInfo, &QWidget::customContextMenuRequested,
		this, &LeftSidePanel::contextInfoMenu);

	// Now playing song
	auto *nowPlayingLayout = new QHBoxLayout();
	nowPlayingLayout->setSpacing(12);
	nowAlbum = new QLabel(this);
	nowAlbum->setFixedSize(64, 64);
	nowPlayingLayout->addWidget(nowAlbum);
	nowPlaying = new QLabel(this);
	nowPlaying->setWordWrap(true);
	nowPlayingLayout->addWidget(nowPlaying);
	layout->addLayout(nowPlayingLayout);

	resetCurrentlyPlaying();

	// Show menu when clicking now playing
	nowPlaying->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(nowPlaying, &QWidget::customContextMenuRequested,
		this, &LeftSidePanel::popupSongMenu);
}

void LeftSidePanel::resetCurrentlyPlaying()
{
	nowAlbum->setPixmap(Icon::get("media-optical-audio").pixmap(nowAlbum->size()));
	nowPlaying->setText("No music playing");
}

void LeftSidePanel::popupSongMenu(const QPoint &pos)
{
	auto track = current.playback.item;
	if (track.name.empty() && track.artists.empty())
	{
		return;
	}

	auto *menu = new SongMenu(track, spotify, parentWidget());
	menu->popup(nowPlaying->mapToGlobal(pos));
}

auto LeftSidePanel::currentContextIcon() const -> QIcon
{
	return Icon::get(QString("view-media-%1")
		.arg(current.playback.context.type.empty()
			? "track"
			: current.playback.context.type == "album"
				? "album-cover"
				: QString::fromStdString(current.playback.context.type)));
}

void LeftSidePanel::updateContextIcon()
{
	if (!settings.general.show_context_info)
	{
		contextIcon->setVisible(false);
		contextInfo->setVisible(false);
		return;
	}

	auto callback = [this](const std::string &currentName)
	{
		this->contextInfo->setText(QString::fromStdString(currentName));
		auto size = this->contextInfo->fontInfo().pixelSize();
		this->contextIcon->setPixmap(currentContextIcon().pixmap(size, size));

		auto show = currentName != "No context";
		this->contextIcon->setVisible(show);
		this->contextInfo->setVisible(show);
	};

	if (current.playback.context.type.empty()
		|| current.playback.context.uri.empty())
	{
		callback("No context");
	}
	else if (current.playback.context.type == "album")
	{
		callback(current.playback.item.album.name);
	}
	else if (current.playback.context.type == "artist")
	{
		callback(lib::spt::entity::combine_names(current.playback.item.artists));
	}
	else
	{
		getPlaylistName(current.playback.context.uri, callback);
	}
}

void LeftSidePanel::contextInfoMenu(const QPoint &pos)
{
	auto *menu = new QMenu(contextInfo);

	if (lib::developer_mode::enabled)
	{
		auto *devContext = menu->addAction(current.context);
		devContext->setEnabled(false);
	}

	auto *open = menu->addAction(currentContextIcon(), QString("Open %1")
		.arg(QString::fromStdString(current.playback.context.type)));
	QAction::connect(open, &QAction::triggered, this, &LeftSidePanel::contextInfoOpen);

	menu->popup(contextInfo->mapToGlobal(pos));
}

void LeftSidePanel::contextInfoOpen(bool /*unused*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	const auto &type = current.playback.context.type;
	const auto uri = lib::strings::split(current.playback.context.uri, ':').back();

	if (type == "album")
	{
		mainWindow->loadAlbum(uri);
	}
	else if (type == "artist")
	{
		mainWindow->openArtist(uri);
	}
	else if (type == "playlist")
	{
		spotify.playlist(uri, [this, mainWindow](const lib::spt::playlist &playlist)
		{
			this->libraryList->setCurrentItem(nullptr);
			this->playlists->setCurrentRow(-1);
			mainWindow->getSongsTree()->load(playlist);
		});
	}
}

auto LeftSidePanel::allArtists() -> std::unordered_set<std::string>
{
	std::unordered_set<std::string> artists;
	for (auto i = 0; i < playlists->count(); i++)
	{
		auto *playlistItem = dynamic_cast<PlaylistListItem *>(playlists->item(i));
		if (playlistItem == nullptr)
		{
			continue;
		}

		auto playlistId = playlistItem->getData().id;
		for (auto &track : cache.get_playlist(playlistId).tracks)
		{
			for (const auto &artist : track.artists)
			{
				artists.insert(artist.name);
			}
		}
	}
	return artists;
}

//region Playlists

auto LeftSidePanel::playlistItem(int index) const -> QListWidgetItem *
{
	return playlists->item(index);
}

auto LeftSidePanel::playlistItemCount() const -> int
{
	return playlists->count();
}

void LeftSidePanel::setCurrentPlaylistItem(int index) const
{
	playlists->setCurrentRow(index);
}

auto LeftSidePanel::currentPlaylist() -> QListWidgetItem *
{
	return playlists->currentItem();
}

void LeftSidePanel::refreshPlaylists()
{
	playlists->refresh();
}

void LeftSidePanel::orderPlaylists(lib::playlist_order order)
{
	playlists->order(order);
}

auto LeftSidePanel::playlist(size_t index) -> lib::spt::playlist &
{
	return playlists->getPlaylists().at(index);
}

auto LeftSidePanel::getPlaylistNameFromSaved(const std::string &id) -> std::string
{
	for (auto &playlist : playlists->getPlaylists())
	{
		if (lib::strings::ends_with(id, playlist.id))
		{
			return playlist.name;
		}
	}
	return std::string();
}

auto LeftSidePanel::getCurrentlyPlaying() const -> const lib::spt::track &
{
	return currentlyPlaying;
}

void LeftSidePanel::setCurrentlyPlaying(const lib::spt::track &value)
{
	nowPlaying->setText(QString::fromStdString(value.details()));
	currentlyPlaying = value;
}

void LeftSidePanel::setAlbumImage(const QPixmap &pixmap)
{
	nowAlbum->setPixmap(pixmap);
}

void LeftSidePanel::getPlaylistName(const std::string &id,
	lib::callback<std::string> &callback)
{
	const auto &name = getPlaylistNameFromSaved(id);
	if (!name.empty())
	{
		callback(name);
	}
	else
	{
		spotify.playlist(lib::spt::api::to_id(id), [callback]
			(const lib::spt::playlist &playlist)
		{
			callback(playlist.name);
		});
	}
}

auto LeftSidePanel::getCurrentLibraryItem() -> QTreeWidgetItem *
{
	return libraryList->currentItem();
}

void LeftSidePanel::setCurrentLibraryItem(QTreeWidgetItem *item)
{
	libraryList->setCurrentItem(item);
}

auto LeftSidePanel::getPlaylists() -> std::vector<lib::spt::playlist> &
{
	return playlists->getPlaylists();
}

void LeftSidePanel::setCurrentPlaylistItem(QListWidgetItem *item)
{
	playlists->setCurrentItem(item);
}

//endregion
