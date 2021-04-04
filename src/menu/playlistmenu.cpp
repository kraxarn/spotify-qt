#include "playlistmenu.hpp"

PlaylistMenu::PlaylistMenu(spt::Spotify &spotify, const lib::spt::playlist &playlist,
	lib::cache &cache, QWidget *parent)
	: parent(parent),
	playlist(playlist),
	QMenu(parent)
{
	auto window = MainWindow::find(parent);
	if (window == nullptr)
		return;

	tracksAction = addAction("... tracks");
	byAction = addAction("By ...");

	tracksLoaded(cache.get_playlist(playlist.id).tracks);
	tracksLoaded(spotify.playlistTracks(playlist));

	addSeparator();
	auto playShuffle = addAction(Icon::get("media-playlist-shuffle"), "Shuffle play");
	QAction::connect(playShuffle, &QAction::triggered,
		[this, playlist, &spotify, window](bool checked)
		{
			if (tracks.empty())
			{
				window->setStatus("No tracks found to shuffle", true);
				return;
			}

			auto initialIndex = lib::random().next_int(0, tracks.size());
			spotify.play_tracks(initialIndex, lib::fmt::format("spotify:playlist:{}",
				playlist.id), [&spotify, window](const std::string &status)
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
		[this, playlist, &spotify, window](bool checked)
		{
			delete editDialog;
			editDialog = new PlaylistEditDialog(&spotify, playlist, -1,
				parentWidget());
			if (editDialog->exec() == QDialog::Accepted)
				window->refreshPlaylists();
		});

	auto share = addMenu(Icon::get("document-share"), "Share");
	auto sharePlaylist = share->addAction("Copy playlist link");
	QAction::connect(sharePlaylist, &QAction::triggered, [window, playlist](bool checked)
	{
		QApplication::clipboard()->setText(QString("https://open.spotify.com/playlist/%1")
			.arg(QString::fromStdString(playlist.id)));
		window->setStatus("Link copied to clipboard");
	});

	auto shareSongOpen = share->addAction("Open in Spotify");
	QAction::connect(shareSongOpen, &QAction::triggered, [this](bool checked)
	{
		Utils::openUrl(QString("https://open.spotify.com/playlist/%1")
				.arg(QString::fromStdString(this->playlist.id)),
			LinkType::Web, this->parent);
	});

	if (lib::developer_mode::enabled)
	{
		auto devMenu = addMenu(Icon::get("folder-txt"), "Developer");
		devMenu->addAction(QString::fromStdString(playlist.id))->setEnabled(false);
		QAction::connect(devMenu->addAction("As JSON"), &QAction::triggered,
			[this](bool checked)
			{
				nlohmann::json json = this->playlist;
				QMessageBox::information(this->parent, "JSON",
					QString::fromStdString(json.dump(4)));
			});
	}
}

void PlaylistMenu::tracksLoaded(const std::vector<lib::spt::track> &items)
{
	tracks = items;

	auto duration = 0;
	for (auto &track : tracks)
		duration += track.duration;
	auto minutes = duration / 1000 / 60;

	if (tracks.size() > 1)
	{
		tracksAction->setText(QString("%1 tracks, %2%3 m")
			.arg(tracks.size())
			.arg(minutes >= 60
				? QString("%1 h ").arg(minutes / 60)
				: QString())
			.arg(minutes % 60));
	}

	auto window = MainWindow::find(this->parentWidget());
	auto isOwner = playlist.is_owner(window->getCurrentUser());
	if (!isOwner && !playlist.owner_name.empty())
	{
		byAction->setText(QString("By %1")
			.arg(QString::fromStdString(playlist.owner_name)));
	}
	editAction->setVisible(isOwner);
}