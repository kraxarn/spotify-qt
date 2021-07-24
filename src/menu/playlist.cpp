#include "menu/playlist.hpp"
#include "mainwindow.hpp"

Menu::Playlist::Playlist(lib::spt::api &spotify, const lib::spt::playlist &playlist,
	lib::cache &cache, QWidget *parent)
	: playlist(playlist),
	cache(cache),
	spotify(spotify),
	QMenu(parent)
{
	auto *window = MainWindow::find(parent);
	if (window == nullptr)
	{
		return;
	}

	tracksAction = addAction("... tracks");
	tracksAction->setEnabled(false);
	byAction = addAction("By ...");
	byAction->setEnabled(false);
	byAction->setVisible(false);

	addSeparator();
	auto *playShuffle = addAction(Icon::get("media-playlist-shuffle"),
		"Shuffle play");
	QAction::connect(playShuffle, &QAction::triggered,
		this, &Menu::Playlist::onShuffle);

	editAction = addAction(Icon::get("document-edit"), "Edit");
	editAction->setVisible(false);
	QAction::connect(editAction, &QAction::triggered,
		this, &Menu::Playlist::onEdit);

	auto *refresh = addAction(Icon::get("view-refresh"), "Refresh");
	refresh->setVisible(dynamic_cast<PlaylistList *>(parentWidget()) != nullptr);
	QAction::connect(refresh, &QAction::triggered,
		this, &Menu::Playlist::onRefresh);

	addMenu(shareMenu());

	if (lib::developer_mode::enabled)
	{
		addMenu(devMenu());
	}

	const auto cached = cache.get_playlist(playlist.id);
	if (!cached.is_null())
	{
		tracksLoaded(cached.tracks);
	}

	if (cached.is_null() || !playlist.is_up_to_date(cached.snapshot))
	{
		spotify.playlist_tracks(playlist, [this](const std::vector<lib::spt::track> &items)
		{
			tracksLoaded(items);
		});
	}
}

auto Menu::Playlist::shareMenu() -> QMenu *
{
	auto *menu = new QMenu("Share", this);
	menu->setIcon(Icon::get("document-share"));

	auto *copyLink = menu->addAction("Copy playlist link");
	QAction::connect(copyLink, &QAction::triggered,
		this, &Menu::Playlist::onCopyLink);

	auto *openInSpotify = menu->addAction("Open in Spotify");
	QAction::connect(openInSpotify, &QAction::triggered,
		this, &Menu::Playlist::onOpenInSpotify);

	return menu;
}

auto Menu::Playlist::devMenu() -> QMenu *
{
	auto *menu = new QMenu("Developer", this);
	menu->setIcon(Icon::get("folder-txt"));

	const auto playlistId = QString::fromStdString(playlist.id);
	QAction::connect(menu->addAction(playlistId), &QAction::triggered,
		this, &Menu::Playlist::onCopyId);

	QAction::connect(menu->addAction("As JSON"), &QAction::triggered,
		this, &Menu::Playlist::onShowJson);

	return menu;
}

void Menu::Playlist::tracksLoaded(const std::vector<lib::spt::track> &items)
{
	constexpr unsigned int sInMin = 60U;
	constexpr unsigned int msInMin = 1000U * sInMin;

	tracks = items;

	auto duration = 0U;
	for (const auto &track : tracks)
	{
		duration += track.duration;
	}
	const auto minutes = duration / msInMin;

	if (!tracks.empty())
	{
		tracksAction->setText(QString("%1 track%2, %3%4 m")
			.arg(tracks.size())
			.arg(tracks.size() == 1 ? "" : "s")
			.arg(minutes >= sInMin
				? QString("%1 h ").arg(minutes / sInMin)
				: QString())
			.arg(minutes % sInMin));
	}

	auto *window = MainWindow::find(this->parentWidget());
	const auto isOwner = playlist.is_owner(window->getCurrentUser());
	if (!isOwner && !playlist.owner_name.empty())
	{
		byAction->setText(QString("By %1")
			.arg(QString::fromStdString(playlist.owner_name)));
		byAction->setVisible(true);
	}
	editAction->setVisible(isOwner);

	if (!items.empty() && !playlist.is_null())
	{
		playlist.tracks = items;
		cache.set_playlist(playlist);
	}
}

auto Menu::Playlist::playlistUrl() const -> QString
{
	return QString("https://open.spotify.com/playlist/%1")
		.arg(QString::fromStdString(playlist.id));
}

void Menu::Playlist::onShuffle(bool /*checked*/)
{
	if (tracks.empty())
	{
		auto *mainWindow = MainWindow::find(parentWidget());
		mainWindow->setStatus("No tracks found to shuffle", true);
		return;
	}

	auto initialIndex = lib::random().next_int(0, static_cast<int>(tracks.size()));
	spotify.play_tracks(initialIndex, lib::spt::api::to_uri("playlist", playlist.id),
		[this](const std::string &status)
		{
			if (!status.empty())
			{
				auto *mainWindow = MainWindow::find(this->parentWidget());
				mainWindow->status(status, true);
				return;
			}

			spotify.set_shuffle(true, [this](const std::string &status)
			{
				if (!status.empty())
				{
					auto *mainWindow = MainWindow::find(this->parentWidget());
					mainWindow->status(status, true);
				}
			});
		});
}

void Menu::Playlist::onEdit(bool /*checked*/)
{
	if (editDialog != nullptr)
	{
		editDialog->deleteLater();
	}

	editDialog = new PlaylistEditDialog(spotify, playlist, -1,
		MainWindow::find(parentWidget()));

	if (editDialog->exec() == QDialog::Accepted)
	{
		auto *mainWindow = MainWindow::find(parentWidget());
		mainWindow->refreshPlaylists();
	}
}

void Menu::Playlist::onRefresh(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->getSongsTree()->refreshPlaylist(playlist);
}

void Menu::Playlist::onCopyLink(bool /*checked*/) const
{
	QApplication::clipboard()->setText(playlistUrl());

	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		mainWindow->setStatus("Link copied to clipboard");
	}
}

void Menu::Playlist::onOpenInSpotify(bool /*checked*/) const
{
	UrlUtils::open(playlistUrl(), LinkType::Web,
		MainWindow::find(parentWidget()));
}

void Menu::Playlist::onCopyId(bool /*checked*/) const
{
	QApplication::clipboard()->setText(QString::fromStdString(playlist.id));
}

void Menu::Playlist::onShowJson(bool /*checked*/) const
{
	nlohmann::json json = playlist;
	QMessageBox::information(MainWindow::find(parentWidget()), "JSON",
		QString::fromStdString(json.dump(4)));
}
