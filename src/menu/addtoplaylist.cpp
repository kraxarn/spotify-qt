#include "menu/addtoplaylist.hpp"

#include "mainwindow.hpp"
#include "util/icon.hpp"
#include "dialog/createplaylist.hpp"
#include "dialog/duplicateplaylisttrack.hpp"

Menu::AddToPlaylist::AddToPlaylist(std::vector<std::string> trackIds, lib::spt::api &spotify,
	const lib::cache &cache, QWidget *parent)
	: QMenu(parent),
	trackIds(std::move(trackIds)),
	spotify(spotify),
	cache(cache)
{
	setIcon(Icon::get(QStringLiteral("list-add")));
	setTitle(QStringLiteral("Add to playlist"));

	auto *newPlaylist = addAction(QStringLiteral("New playlist"));
	newPlaylist->setData({});

	QMenu::connect(this, &QMenu::aboutToShow,
		this, &Menu::AddToPlaylist::onAboutToShow);

	QMenu::connect(this, &QMenu::triggered,
		this, &Menu::AddToPlaylist::onTriggered);
}

void Menu::AddToPlaylist::onAboutToShow()
{
	auto *mainWindow = MainWindow::find(parentWidget());
	const auto &currentUserId = mainWindow->getCurrentUser().id;

	addSeparator();
	for (auto &playlist: cache.get_playlists())
	{
		if (!playlist.collaborative && playlist.owner_id != currentUserId)
		{
			continue;
		}

		// Create main action
		auto *action = addAction(QString::fromStdString(playlist.name));
		action->setData(QString::fromStdString(playlist.id));
	}
}

void Menu::AddToPlaylist::onTriggered(QAction *action)
{
	const auto &data = action->data();

	if (!data.isValid())
	{
		addToNewPlaylist();
	}
	else
	{
		addToPlaylist(data.toString().toStdString());
	}
}

auto Menu::AddToPlaylist::getFilteredTrackIds(const std::vector<lib::spt::track> &tracks)
-> std::vector<std::string>
{
	std::vector<std::string> results;

	for (const auto &trackId: trackIds)
	{
		for (const auto &track: tracks)
		{
			if (trackId == track.id)
			{
				break;
			}
		}

		results.push_back(trackId);
	}

	return results;
}

void Menu::AddToPlaylist::addToNewPlaylist()
{
	auto *createPlaylist = new Dialog::CreatePlaylist(trackIds, spotify, window());
	createPlaylist->open();
}

void Menu::AddToPlaylist::addToPlaylist(const std::string &playlistId)
{
	spotify.playlist(playlistId, [this, playlistId](const lib::spt::playlist &playlist)
	{
		const auto playlistName = QString::fromStdString(playlist.name);

		// Check existing tracks for any overlap
		this->spotify.playlist_tracks(playlist,
			[this, playlistId, playlistName](const std::vector<lib::spt::track> &playlistTracks)
			{
				auto filteredTrackIds = getFilteredTrackIds(playlistTracks);
				auto response = DuplicateTrackResponse::AddAll;

				if (playlistTracks.size() != filteredTrackIds.size())
				{
					auto *dialog = new Dialog::DuplicatePlaylistTrack(window());
					dialog->exec();
					response = dialog->response();
				}

				if (response == DuplicateTrackResponse::None)
				{
					return;
				}

				// Actually add
				const auto &toAdd = response == DuplicateTrackResponse::AddAll
					? trackIds
					: filteredTrackIds;

				std::vector<std::string> trackUris;
				trackUris.reserve(toAdd.size());

				for (const auto &trackId: toAdd)
				{
					trackUris.push_back(lib::spt::api::to_uri("track", trackId));
				}

				spotify.add_to_playlist(playlistId, trackUris,
					[playlistName](const std::string &result)
					{
						if (!result.empty())
						{
							StatusMessage::error(QString("Failed to add track to playlist: %1")
								.arg(QString::fromStdString(result)));
							return;
						}

						StatusMessage::info(QString("Added to %1").arg(playlistName));
					});
			});
	});
}
