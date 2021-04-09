#include "playlistmenu.hpp"

PlaylistMenu::PlaylistMenu(spt::Spotify &spotify, const lib::spt::playlist &playlist,
	lib::cache &cache, QWidget *parent)
	: parent(parent),
	playlist(playlist),
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

	addSeparator();
	auto *playShuffle = addAction(Icon::get("media-playlist-shuffle"),
		"Shuffle play");
	QAction::connect(playShuffle, &QAction::triggered,
		[this, playlist, &spotify, window](bool /*checked*/)
		{
			if (tracks.empty())
			{
				window->setStatus("No tracks found to shuffle", true);
				return;
			}

			auto initialIndex = lib::random().next_int(0, tracks.size());
			spotify.play_tracks(initialIndex, lib::spt::api::to_uri("playlist", playlist.id),
				[&spotify, window](const std::string &status)
				{
					if (!status.empty())
					{
						window->status(status, true);
						return;
					}

					spotify.set_shuffle(true, [window](const std::string &status)
					{
						window->status(status, true);
					});
				});
		});

	editAction = addAction(Icon::get("document-edit"), "Edit");
	editAction->setVisible(false);
	QAction::connect(editAction, &QAction::triggered,
		[this, playlist, &spotify, window](bool /*checked*/)
		{
			delete editDialog;
			editDialog = new PlaylistEditDialog(&spotify, playlist, -1,
				parentWidget());
			if (editDialog->exec() == QDialog::Accepted)
			{
				window->refreshPlaylists();
			}
		});

	auto *share = addMenu(Icon::get("document-share"), "Share");
	auto *sharePlaylist = share->addAction("Copy playlist link");
	QAction::connect(sharePlaylist, &QAction::triggered, [window, playlist](bool /*checked*/)
	{
		QApplication::clipboard()->setText(QString("https://open.spotify.com/playlist/%1")
			.arg(QString::fromStdString(playlist.id)));
		window->setStatus("Link copied to clipboard");
	});

	auto *shareSongOpen = share->addAction("Open in Spotify");
	QAction::connect(shareSongOpen, &QAction::triggered, [this](bool /*checked*/)
	{
		Utils::openUrl(QString("https://open.spotify.com/playlist/%1")
				.arg(QString::fromStdString(this->playlist.id)),
			LinkType::Web, this->parent);
	});

	if (lib::developer_mode::enabled)
	{
		auto *devMenu = addMenu(Icon::get("folder-txt"), "Developer");

		const auto playlistId = QString::fromStdString(playlist.id);
		QAction::connect(devMenu->addAction(playlistId), &QAction::triggered,
			[playlistId](bool /*checked*/)
			{
				QApplication::clipboard()->setText(playlistId);
			});

		QAction::connect(devMenu->addAction("As JSON"), &QAction::triggered,
			[this](bool /*checked*/)
			{
				nlohmann::json json = this->playlist;
				QMessageBox::information(this->parent, "JSON",
					QString::fromStdString(json.dump(4)));
			});
	}

	tracksLoaded(cache.get_playlist(playlist.id).tracks);
	spotify.playlist_tracks(playlist, [this](const std::vector<lib::spt::track> &items)
	{
		tracksLoaded(items);
	});
}

void PlaylistMenu::tracksLoaded(const std::vector<lib::spt::track> &items)
{
	constexpr int sInMin = 60;
	constexpr int msInMin = 1000 * sInMin;

	tracks = items;

	auto duration = 0;
	for (const auto &track : tracks)
	{
		duration += track.duration;
	}
	const auto minutes = duration / msInMin;

	if (tracks.size() > 1)
	{
		tracksAction->setText(QString("%1 tracks, %2%3 m")
			.arg(tracks.size())
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
	}
	editAction->setVisible(isOwner);
}