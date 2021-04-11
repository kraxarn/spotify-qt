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
	auto layout = new QVBoxLayout(this);
	setMaximumWidth(250); // TODO: ?

	// Library
	libraryList = new LibraryList(spotify, parent);
	auto library = Utils::createGroupBox(QVector<QWidget *>() << libraryList, parent);
	library->setTitle("Library");
	layout->addWidget(library);

	// Playlists
	playlists = new PlaylistList(spotify, settings, cache, parent);
	refreshPlaylists();
	auto playlistContainer = Utils::createGroupBox(QVector<QWidget *>() << playlists, parent);
	playlistContainer->setTitle("Playlists");
	layout->addWidget(playlistContainer);

	// Current context info
	auto contextLayout = new QHBoxLayout();
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
	auto nowPlayingLayout = new QHBoxLayout();
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
	if (track.name.empty() && track.artist.empty())
		return;
	(new SongMenu(track, spotify, parentWidget()))->popup(nowPlaying->mapToGlobal(pos));
}

QIcon LeftSidePanel::currentContextIcon() const
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
		callback("No context");
	else if (current.playback.context.type == "album")
		callback(current.playback.item.album);
	else if (current.playback.context.type == "artist")
		callback(current.playback.item.artist);
	else
		getPlaylistName(current.playback.context.uri, callback);
}

void LeftSidePanel::contextInfoMenu(const QPoint &pos)
{
	auto menu = new QMenu(contextInfo);

	if (lib::developer_mode::enabled)
	{
		auto devContext = menu->addAction(current.context);
		devContext->setEnabled(false);
	}

	auto open = menu->addAction(currentContextIcon(), QString("Open %1")
		.arg(QString::fromStdString(current.playback.context.type)));
	QAction::connect(open, &QAction::triggered, this, &LeftSidePanel::contextInfoOpen);

	menu->popup(contextInfo->mapToGlobal(pos));
}

void LeftSidePanel::contextInfoOpen(bool)
{
	auto mainWindow = MainWindow::find(parentWidget());
	auto type = current.playback.context.type;
	auto uri = lib::strings::split(current.playback.context.uri, ':').back();

	if (type == "album")
		mainWindow->loadAlbum(uri);
	else if (type == "artist")
		mainWindow->openArtist(uri);
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

std::unordered_set<std::string> LeftSidePanel::allArtists()
{
	std::unordered_set<std::string> artists;
	for (auto i = 0; i < playlists->count(); i++)
	{
		auto playlistId = playlists->item(i)->data(RolePlaylistId).toString().toStdString();
		for (auto &track : cache.get_playlist(playlistId).tracks)
			artists.insert(track.artist);
	}
	return artists;
}

//region Playlists

QListWidgetItem *LeftSidePanel::playlistItem(int index) const
{
	return playlists->item(index);
}

int LeftSidePanel::playlistItemCount() const
{
	return playlists->count();
}

void LeftSidePanel::setCurrentPlaylistItem(int index) const
{
	playlists->setCurrentRow(index);
}

QListWidgetItem *LeftSidePanel::currentPlaylist()
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

int LeftSidePanel::playlistCount() const
{
	return playlists->getPlaylists().size();
}

lib::spt::playlist &LeftSidePanel::playlist(size_t index)
{
	return playlists->getPlaylists().at(index);
}

std::string LeftSidePanel::getPlaylistNameFromSaved(const std::string &id)
{
	for (auto &playlist : playlists->getPlaylists())
	{
		if (lib::strings::ends_with(id, playlist.id))
			return playlist.name;
	}
	return std::string();
}

QString LeftSidePanel::getCurrentlyPlaying()
{
	return nowPlaying->text();
}

void LeftSidePanel::setCurrentlyPlaying(const QString &value)
{
	nowPlaying->setText(value);
}

void LeftSidePanel::setAlbumImage(const QPixmap &pixmap)
{
	nowAlbum->setPixmap(pixmap);
}

void LeftSidePanel::getPlaylistName(const std::string &id,
	lib::callback<std::string> &callback)
{
	auto name = getPlaylistNameFromSaved(id);
	if (!name.empty())
		callback(name);
	else
	{
		spotify.playlist(lib::spt::api::to_id(id),
			[callback](const lib::spt::playlist &playlist)
		{
			callback(playlist.name);
		});
	}
}

QTreeWidgetItem *LeftSidePanel::getCurrentLibraryItem()
{
	return libraryList->currentItem();
}

void LeftSidePanel::setCurrentLibraryItem(QTreeWidgetItem *item)
{
	libraryList->setCurrentItem(item);
}

std::vector<lib::spt::playlist> &LeftSidePanel::getPlaylists()
{
	return playlists->getPlaylists();
}

//endregion
