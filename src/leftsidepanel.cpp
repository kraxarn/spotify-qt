#include "leftsidepanel.hpp"

#include "mainwindow.hpp"

LeftSidePanel::LeftSidePanel(spt::Spotify &spotify, lib::settings &settings,
	spt::Current &current, QWidget *parent)
	: spotify(spotify),
	settings(settings),
	current(current),
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
	playlists = new PlaylistList(spotify, parent);
	refreshPlaylists();
	auto playlistContainer = Utils::createGroupBox(QVector<QWidget *>() << playlists, parent);
	playlistContainer->setTitle("Playlists");
	layout->addWidget(playlistContainer);

	// Current context info
	auto contextLayout = new QHBoxLayout();
	contextIcon = new QLabel();
	contextIcon->setVisible(false);
	contextInfo = new QLabel();
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
	nowAlbum->setPixmap(Icon::get("media-optical-audio").pixmap(nowAlbum->size()));
	nowPlayingLayout->addWidget(nowAlbum);
	nowPlaying = new QLabel("No music playing", this);
	nowPlaying->setWordWrap(true);
	nowPlayingLayout->addWidget(nowPlaying);
	layout->addLayout(nowPlayingLayout);

	// Show menu when clicking now playing
	nowPlaying->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
	QLabel::connect(nowPlaying, &QWidget::customContextMenuRequested,
		this, &LeftSidePanel::popupSongMenu);
}

void LeftSidePanel::popupSongMenu(const QPoint &pos)
{
	auto track = current.playback.item;
	if (track.name.isEmpty() && track.artist.isEmpty())
		return;
	(new SongMenu(track, spotify, parentWidget()))->popup(nowPlaying->mapToGlobal(pos));
}

int LeftSidePanel::latestTrack(const QVector<spt::Track> &tracks)
{
	auto latest = 0;
	for (int i = 0; i < tracks.length(); i++)
	{
		if (tracks[i].addedAt > tracks[latest].addedAt)
			latest = i;
	}
	return latest;
}

QIcon LeftSidePanel::currentContextIcon() const
{
	return Icon::get(QString("view-media-%1")
		.arg(current.playback.contextType.isEmpty()
			? "track"
			: current.playback.contextType == "album"
				? "album-cover"
				: current.playback.contextType));
}

void LeftSidePanel::updateContextIcon()
{
	if (!settings.general.show_context_info)
	{
		contextIcon->setVisible(false);
		contextInfo->setVisible(false);
		return;
	}

	auto currentName = current.playback.contextType.isEmpty()
		|| current.playback.contextUri.isEmpty()
		? "No context"
		: current.playback.contextType == "album"
			? current.playback.item.album
			: current.playback.contextType == "artist"
				? current.playback.item.artist
				: getPlaylistName(current.playback.contextUri);

	contextInfo->setText(currentName);
	auto size = contextInfo->fontInfo().pixelSize();
	contextIcon->setPixmap(currentContextIcon().pixmap(size, size));

	auto show = currentName != "No context";
	contextIcon->setVisible(show);
	contextInfo->setVisible(show);
}

void LeftSidePanel::contextInfoMenu(const QPoint &pos)
{
	auto menu = new QMenu(contextInfo);

	if (lib::developer_mode::enabled)
	{
		auto devContext = menu->addAction(current.context);
		devContext->setEnabled(false);
	}

	auto open = menu->addAction(currentContextIcon(),
		QString("Open %1").arg(current.playback.contextType));
	QAction::connect(open, &QAction::triggered, this, &LeftSidePanel::contextInfoOpen);

	menu->popup(contextInfo->mapToGlobal(pos));
}

void LeftSidePanel::contextInfoOpen(bool)
{
	auto mainWindow = MainWindow::find(parentWidget());
	auto type = current.playback.contextType;
	auto uri = current.playback.contextUri.split(':').last();

	if (type == "album")
		mainWindow->loadAlbum(uri);
	else if (type == "artist")
		mainWindow->openArtist(uri);
	else if (type == "playlist")
	{
		auto playlist = spotify.playlist(uri);
		libraryList->setCurrentItem(nullptr);
		playlists->setCurrentRow(-1);
		mainWindow->loadPlaylist(playlist);
	}
}

QSet<QString> LeftSidePanel::allArtists()
{
	QSet<QString> artists;
	for (auto i = 0; i < playlists->count(); i++)
		for (auto &track : MainWindow::find(parentWidget())
			->playlistTracks(playlists->item(i)->data(RolePlaylistId).toString()))
			artists << track.artist;
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
	QListWidgetItem *currentItem = nullptr;
	QString lastItem;
	if (currentPlaylist() != nullptr)
		lastItem = currentPlaylist()->data(RolePlaylistId).toString();

	sptPlaylists = spotify.playlists();

	// Add all playlists
	playlists->clear();
	auto i = 0;
	QTextDocument doc;
	for (auto &playlist : sptPlaylists)
	{
		auto item = new QListWidgetItem(playlist.name, playlists);

		doc.setHtml(playlist.description);
		item->setToolTip(doc.toPlainText());

		item->setData(RolePlaylistId, playlist.id);
		item->setData(RoleIndex, i++);

		if (playlist.id == lastItem)
			currentItem = item;
	}

	// Sort
	if (settings.general.playlist_order != lib::playlist_order_default)
		orderPlaylists(settings.general.playlist_order);

	if (currentItem != nullptr)
		playlists->setCurrentItem(currentItem);
}

void LeftSidePanel::orderPlaylists(lib::playlist_order order)
{
	QList<QListWidgetItem *> items;
	items.reserve(playlists->count());

	auto i = 0;
	while (playlists->item(0) != nullptr)
		items.insert(i, playlists->takeItem(0));

	QMap<QString, int> customOrder;
	MainWindow *mainWindow;

	switch (order)
	{
		case lib::playlist_order_default:
			std::sort(items.begin(), items.end(), [](QListWidgetItem *i1, QListWidgetItem *i2)
			{
				return i1->data(RoleIndex).toInt() < i2->data(RoleIndex).toInt();
			});
			break;

		case lib::playlist_order_alphabetical:
			std::sort(items.begin(), items.end(), [](QListWidgetItem *i1, QListWidgetItem *i2)
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

			std::sort(items.begin(), items.end(),
				[mainWindow](QListWidgetItem *i1, QListWidgetItem *i2)
				{
					auto t1 = mainWindow->playlistTracks(i1
						->data(DataRole::RolePlaylistId).toString());
					auto t2 = mainWindow->playlistTracks(i2
						->data(DataRole::RolePlaylistId).toString());

					return t1.length() > 0 && t2.length() > 0
						? t1.at(latestTrack(t1)).addedAt > t2.at(latestTrack(t2)).addedAt
						: false;
				});
			break;

		case lib::playlist_order_custom:
			i = 0;
			for (auto &playlist : settings.general.custom_playlist_order)
				customOrder[QString::fromStdString(playlist)] = i++;
			std::sort(items.begin(), items.end(),
				[customOrder](QListWidgetItem *i1, QListWidgetItem *i2)
				{
					auto id1 = i1->data(DataRole::RolePlaylistId).toString();
					auto id2 = i2->data(DataRole::RolePlaylistId).toString();

					return customOrder.contains(id1) && customOrder.contains(id2)
						? customOrder[id1] < customOrder[id2]
						: false;
				});
			break;
	}

	for (auto item : items)
		playlists->addItem(item);
}

int LeftSidePanel::playlistCount() const
{
	return sptPlaylists.length();
}

spt::Playlist &LeftSidePanel::playlist(size_t index)
{
	return sptPlaylists[index];
}

QString LeftSidePanel::getPlaylistNameFromSaved(const QString &id)
{
	for (auto &playlist : sptPlaylists)
	{
		if (id.endsWith(playlist.id))
			return playlist.name;
	}
	return QString();
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

QString LeftSidePanel::getPlaylistName(const QString &id)
{
	auto name = getPlaylistNameFromSaved(id);
	if (!name.isEmpty())
		return name;
	return spotify.playlist(id.split(':').last()).name;
}

QTreeWidgetItem *LeftSidePanel::getCurrentLibraryItem()
{
	return libraryList->currentItem();
}

void LeftSidePanel::setCurrentLibraryItem(QTreeWidgetItem *item)
{
	libraryList->setCurrentItem(item);
}

QVector<spt::Playlist> &LeftSidePanel::getPlaylists()
{
	return sptPlaylists;
}

//endregion