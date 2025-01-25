#include "createplaylist.hpp"
#include "mainwindow.hpp"

#include <QVBoxLayout>
#include <QMessageBox>

Dialog::CreatePlaylist::CreatePlaylist(std::vector<std::string> trackIds,
	lib::spt::api &spotify, QWidget *parent)
	: Base(parent),
	trackIds(std::move(trackIds)),
	spotify(spotify)
{
	setTitle("Create playlist");

	auto *layout = Base::layout<QVBoxLayout>();
	layout->addWidget(new QLabel("Playlist name", this));

	playlistNameEdit = new QLineEdit(this);
	layout->addWidget(playlistNameEdit);

	addAction(DialogAction::Ok);
	addAction(DialogAction::Cancel);
}

void Dialog::CreatePlaylist::showEvent(QShowEvent *event)
{
	QDialog::showEvent(event);

	auto *mainWindow = MainWindow::find(parentWidget());
	if (mainWindow != nullptr)
	{
		defaultName = QString("Playlist #%1")
			.arg(mainWindow->getPlaylistItemCount() + 1);

		playlistNameEdit->setPlaceholderText(defaultName);
	}
}

void Dialog::CreatePlaylist::onOk(bool /*checked*/)
{
	okButton()->setEnabled(false);

	const auto name = getPlaylistName().toStdString();
	if (name.empty())
	{
		okButton()->setEnabled(true);
		return;
	}

	const auto description = std::optional<std::string>();
	const auto isPublic = std::optional<bool>();
	const auto isCollaborative = std::optional<bool>();

	spotify.create_playlist(name, description, isPublic, isCollaborative,
		[this](const lib::spt::playlist &playlist)
		{
			if (playlist.is_null())
			{
				showError(QStringLiteral("Failed to create playlist"));
				return;
			}

			const auto trackUris = getTrackUris();
			if (trackUris.empty())
			{
				auto *mainWindow = MainWindow::find(parentWidget());
				mainWindow->refreshPlaylists();

				StatusMessage::info(QStringLiteral("Playlist created"));
				Base::onOk({});
				return;
			}

			const auto playlistName = QString::fromStdString(playlist.name);

			spotify.add_to_playlist(playlist.id, trackUris,
				[this, playlistName](const std::string &result)
				{
					if (!result.empty())
					{
						showError(QString::fromStdString(result));
						return;
					}

					auto *mainWindow = MainWindow::find(parentWidget());
					mainWindow->refreshPlaylists();
					StatusMessage::info(QString("Added to %1").arg(playlistName));
					Base::onOk({});
				});
		});
}

auto Dialog::CreatePlaylist::getPlaylistName() const -> QString
{
	auto playlistText = playlistNameEdit->text();
	if (!playlistText.isEmpty())
	{
		return playlistText;
	}

	return defaultName;
}

auto Dialog::CreatePlaylist::getTrackUris() const -> std::vector<std::string>
{
	std::vector<std::string> uris;
	uris.reserve(trackIds.size());

	for (const auto &trackId: trackIds)
	{
		uris.push_back(lib::spt::id_to_uri("track", trackId));
	}

	return uris;
}

void Dialog::CreatePlaylist::showError(const QString &message)
{
	okButton()->setEnabled(true);
	QMessageBox::critical(this, QStringLiteral("Error"), message);
}
