#include "menu/playlist.hpp"
#include "mainwindow.hpp"
#include "dialog/trackscache.hpp"

Menu::Playlist::Playlist(lib::spt::api &spotify, const lib::spt::playlist &playlist,
	lib::cache &cache, QWidget *parent)
	: QMenu(parent),
	playlist(playlist),
	cache(cache),
	spotify(spotify)
{
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
	refresh->setVisible(dynamic_cast<List::Playlist *>(parentWidget()) != nullptr);
	QAction::connect(refresh, &QAction::triggered,
		this, &Menu::Playlist::onRefresh);

	followAction = addAction(Icon::get("non-starred-symbolic"), "Follow");
	followAction->setEnabled(false);

	QAction::connect(followAction, &QAction::triggered,
		this, &Menu::Playlist::onFollow);

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

	const auto *mainWindow = MainWindow::find(parentWidget());
	const auto &currentUser = mainWindow != nullptr
		? mainWindow->getCurrentUser()
		: lib::spt::user();

	if (cached.is_null() || !playlist.is_up_to_date(cached.snapshot, currentUser))
	{
		spotify.playlist_tracks(playlist, [this](const std::vector<lib::spt::track> &items)
		{
			tracksLoaded(items);
		});
	}
}

void Menu::Playlist::showEvent(QShowEvent *event)
{
	QWidget::showEvent(event);

	auto *mainWindow = MainWindow::find(parentWidget());

	spotify.is_following_playlist(playlist.id, {
		mainWindow->getCurrentUser().id,
	}, [this](const std::vector<bool> &follows)
	{
		isFollowingLoaded(follows);
	});
}

auto Menu::Playlist::shareMenu() -> QMenu *
{
	auto *menu = new QMenu("Share", this);
	menu->setIcon(Icon::get("document-share"));

	auto *copyLink = menu->addAction("Copy playlist link");
	QAction::connect(copyLink, &QAction::triggered,
		this, &Menu::Playlist::onCopyLink);

	auto *copyName = menu->addAction("Copy playlist name");
	QAction::connect(copyName, &QAction::triggered,
		this, &Menu::Playlist::onCopyName);

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
	auto *copyId = menu->addAction(QString("Copy ID: %1").arg(playlistId));

	QAction::connect(copyId, &QAction::triggered,
		this, &Menu::Playlist::onCopyId);

	auto *openJson = menu->addAction(QStringLiteral("View as JSON"));
	QAction::connect(openJson, &QAction::triggered,
		this, &Menu::Playlist::onShowJson);

	auto *openTracks = menu->addAction(QStringLiteral("View cached tracks"));
	QAction::connect(openTracks, &QAction::triggered,
		this, &Menu::Playlist::onShowTracks);

	return menu;
}

void Menu::Playlist::tracksLoaded(const std::vector<lib::spt::track> &items)
{
	constexpr unsigned int sInMin = 60U;
	constexpr unsigned int msInMin = 1000U * sInMin;

	tracks = items;

	auto duration = 0U;
	for (const auto &track: tracks)
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

void Menu::Playlist::isFollowingLoaded(const std::vector<bool> &follows)
{
	if (follows.empty())
	{
		followAction->setVisible(false);
		return;
	}

	followAction->setEnabled(true);
	if (!follows.at(0))
	{
		return;
	}

	followAction->setIcon(Icon::get(QStringLiteral("starred-symbolic")));
	followAction->setText(QStringLiteral("Unfollow"));
}

auto Menu::Playlist::playlistUrl() const -> QString
{
	return QString("https://open.spotify.com/playlist/%1")
		.arg(QString::fromStdString(playlist.id));
}

auto Menu::Playlist::playlistName() const -> QString
{
	return QString::fromStdString(
		lib::fmt::format("\"{}\" by {}", playlist.name, playlist.owner_name));
}

void Menu::Playlist::onShuffle(bool /*checked*/)
{
	if (tracks.empty())
	{
		StatusMessage::warn(QStringLiteral("No tracks found to shuffle"));
		return;
	}

	auto initialIndex = lib::random().next_int(0, static_cast<int>(tracks.size()));
	spotify.play_tracks(initialIndex, lib::spt::id_to_uri("playlist", playlist.id),
		[this](const std::string &status)
		{
			if (!status.empty())
			{
				StatusMessage::error(QString::fromStdString(status));
				return;
			}

			spotify.set_shuffle(true, [](const std::string &status)
			{
				if (!status.empty())
				{
					StatusMessage::error(QString::fromStdString(status));
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

	editDialog = new Dialog::EditPlaylist(spotify, playlist, -1,
		MainWindow::find(parentWidget()));

	QDialog::connect(editDialog, &Dialog::EditPlaylist::playlistSaved,
		this, &Menu::Playlist::onPlaylistSaved);

	editDialog->open();
}

void Menu::Playlist::onRefresh(bool /*checked*/)
{
	auto *mainWindow = MainWindow::find(parentWidget());
	mainWindow->getSongsTree()->refreshPlaylist(playlist);
}

void Menu::Playlist::onFollow(bool /*checked*/)
{
	const auto isFollowing = followAction->text() == "Unfollow";

	auto callback = [this, isFollowing](const std::string &status)
	{
		if (!status.empty())
		{
			StatusMessage::error(QString("Failed to %1: %2")
				.arg(isFollowing
					? QStringLiteral("unfollow")
					: QStringLiteral("follow"))
				.arg(QString::fromStdString(status)));
			return;
		}

		auto *mainWindow = MainWindow::find(parentWidget());
		mainWindow->refreshPlaylists();
	};

	if (isFollowing)
	{
		spotify.unfollow_playlist(playlist.id, callback);
	}
	else
	{
		spotify.follow_playlist(playlist.id, callback);
	}
}

void Menu::Playlist::onCopyLink(bool /*checked*/) const
{
	QApplication::clipboard()->setText(playlistUrl());
	StatusMessage::info(QStringLiteral("Link copied to clipboard"));
}

void Menu::Playlist::onCopyName(bool /*checked*/) const
{
	QApplication::clipboard()->setText(playlistName());
	StatusMessage::info(QStringLiteral("Name copied to clipboard"));
}

void Menu::Playlist::onOpenInSpotify(bool /*checked*/) const
{
	Url::open(playlistUrl(), LinkType::Web, MainWindow::find(parentWidget()));
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

void Menu::Playlist::onShowTracks(bool /*checked*/) const
{
	auto *mainWindow = MainWindow::find(parent());
	auto *dialog = new Dialog::TracksCache(mainWindow);
	dialog->load(playlist.tracks);
	dialog->open();
}

void Menu::Playlist::onPlaylistSaved()
{
	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		mainWindow->refreshPlaylists();
	}
}
