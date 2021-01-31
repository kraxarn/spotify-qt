#include "playlistmenu.hpp"

PlaylistMenu::PlaylistMenu(spt::Spotify &spotify, const spt::Playlist &playlist, QWidget *parent)
	: parent(parent),
	playlist(playlist),
	QMenu(parent)
{
	auto window = MainWindow::find(parent);
	if (window == nullptr)
		return;

	auto tracks = window->playlistTracks(playlist.id);
	if (tracks.isEmpty())
		playlist.loadTracks(spotify, tracks);

	auto duration = 0;
	for (auto &track : tracks)
		duration += track.duration;
	auto minutes = duration / 1000 / 60;
	if (tracks.length() > 1)
	{
		addAction(QString("%1 tracks, %2%3 m")
			.arg(tracks.length())
			.arg(minutes >= 60
				? QString("%1 h ").arg(minutes / 60)
				: QString())
			.arg(minutes % 60))->setEnabled(false);
	}

	auto isOwner = playlist.isOwner(window->getCurrentUser());
	if (!isOwner && !playlist.ownerName.isEmpty())
		addAction(QString("By %1").arg(playlist.ownerName))->setEnabled(false);

	addSeparator();
	auto playShuffle = addAction(Icon::get("media-playlist-shuffle"), "Shuffle play");
	QAction::connect(playShuffle, &QAction::triggered,
		[tracks, playlist, &spotify, window](bool checked)
		{
			if (tracks.isEmpty())
			{
				window->setStatus("No tracks found to shuffle", true);
				return;
			}

			auto initialIndex = 0;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
			initialIndex = QRandomGenerator::global()->bounded(0, tracks.length());
#endif
			auto status = spotify.playTracks(initialIndex,
				QString("spotify:playlist:%1").arg(playlist.id));

			if (status.isEmpty())
				status = spotify.setShuffle(true);
			if (!status.isEmpty())
				window->setStatus(status, true);
		});

	if (isOwner)
	{
		QAction::connect(addAction(Icon::get("document-edit"), "Edit"), &QAction::triggered,
			[this, playlist, &spotify, window](bool checked)
			{
				delete editDialog;
				editDialog = new PlaylistEditDialog(&spotify, playlist, -1, parentWidget());
				if (editDialog->exec() == QDialog::Accepted)
					window->refreshPlaylists();
			});
	}

	auto share = addMenu(Icon::get("document-share"), "Share");
	auto sharePlaylist = share->addAction("Copy playlist link");
	QAction::connect(sharePlaylist, &QAction::triggered, [window, playlist](bool checked)
	{
		QApplication::clipboard()->setText(QString("https://open.spotify.com/playlist/%1")
			.arg(QString(playlist.id)));
		window->setStatus("Link copied to clipboard");
	});

	auto shareSongOpen = share->addAction("Open in Spotify");
	QAction::connect(shareSongOpen, &QAction::triggered, [this](bool checked)
	{
		Utils::openUrl(QString("https://open.spotify.com/playlist/%1")
			.arg(QString(this->playlist.id)), LinkType::Web, this->parent);
	});

	if (lib::developer_mode::enabled)
	{
		auto devMenu = addMenu(Icon::get("folder-txt"), "Developer");
		devMenu->addAction(playlist.id)->setEnabled(false);
		QAction::connect(devMenu->addAction("As JSON"), &QAction::triggered,
			[this](bool checked)
			{
				QMessageBox::information(this->parent, "JSON",
					QJsonDocument(this->playlist.toJson(QJsonArray()))
						.toJson(QJsonDocument::Indented));
			});
	}
}

PlaylistMenu::PlaylistMenu(spt::Spotify &spotify, const QString &playlistId, QWidget *parent)
	: PlaylistMenu(spotify, spotify.playlist(playlistId), parent)
{
}
