#include "dialog/addtoplaylist.hpp"
#include "widget/statusmessage.hpp"

#include "lib/set.hpp"

#include <QPushButton>
#include <utility>

Dialog::AddToPlaylist::AddToPlaylist(lib::spt::api &spotify, lib::spt::playlist playlist,
	const std::vector<lib::spt::track> &playlistTracks,
	std::vector<std::string> trackIds, QWidget *parent)
	: Base(parent),
	spotify(spotify),
	playlist(std::move(playlist)),
	trackIdsToAdd(std::move(trackIds))
{
	setTitle(QStringLiteral("Duplicate"));

	for (const auto &playlistTrack: playlistTracks)
	{
		playlistTrackIds.insert(playlistTrack.id);
	}

	auto *layout = Base::layout<QVBoxLayout>();
	const auto isSingleTrack = trackIdsToAdd.size() == 1;

	const auto text = isSingleTrack
		? QStringLiteral("Track is already in the destination playlist. "
						 "Do you want to add it anyway?")
		: QStringLiteral("One or more tracks are already in the destination playlist. "
						 "Do you want to add all missing tracks, or add all tracks anyway?");

	auto *label = new QLabel(text, this);
	layout->addWidget(label);

	addAction(DialogAction::Cancel);

	if (!isSingleTrack)
	{
		auto *addMissing = addButton(QStringLiteral("Add missing"),
			QDialogButtonBox::AcceptRole);

		QPushButton::connect(addMissing, &QPushButton::clicked,
			this, &Dialog::AddToPlaylist::onAddMissingClicked);
	}

	const auto addAllText = isSingleTrack
		? QStringLiteral("Add anyway")
		: QStringLiteral("Add all");

	auto *addAll = addButton(addAllText, QDialogButtonBox::ActionRole);

	QPushButton::connect(addAll, &QPushButton::clicked,
		this, &Dialog::AddToPlaylist::onAddAllClicked);
}

void Dialog::AddToPlaylist::ask(lib::spt::api &spotify, const lib::spt::playlist &playlist,
	const std::vector<std::string> &trackIds, QWidget *parent)
{
	spotify.playlist_tracks(playlist,
		[&spotify, playlist, trackIds, parent](const std::vector<lib::spt::track> &playlistTracks)
		{
			auto *dialog = new Dialog::AddToPlaylist(spotify, playlist,
				playlistTracks, trackIds, parent);

			if (!dialog->shouldAsk())
			{
				dialog->addTracks(trackIds);
				return;
			}

			dialog->open();
		});
}

auto Dialog::AddToPlaylist::shouldAsk() -> bool
{
	// Any of the tracks to add already exists in the playlist
	return std::any_of(trackIdsToAdd.cbegin(), trackIdsToAdd.cend(),
		[this](const std::string &trackId) -> bool
		{
			return lib::set::contains(playlistTrackIds, trackId);
		});
}

auto Dialog::AddToPlaylist::getTrackIdsNotInPlaylist() -> std::vector<std::string>
{
	std::vector<std::string> results;

	for (const auto &trackId: trackIdsToAdd)
	{
		if (!lib::set::contains(playlistTrackIds, trackId))
		{
			results.push_back(trackId);
		}
	}

	return results;
}

void Dialog::AddToPlaylist::addTracks(const std::vector<std::string> &trackIds)
{
	std::vector<std::string> trackUris;
	trackUris.reserve(trackUris.size());

	for (const auto &trackId: trackIds)
	{
		trackUris.push_back(lib::spt::id_to_uri("track", trackId));
	}

	spotify.add_to_playlist(playlist.id, trackUris,
		[this](const std::string &result)
		{
			if (!result.empty())
			{
				StatusMessage::error(QString("Failed to add track to playlist: %1")
					.arg(QString::fromStdString(result)));
				return;
			}

			StatusMessage::info(QString("Added to %1")
				.arg(QString::fromStdString(playlist.name)));
		});
}

void Dialog::AddToPlaylist::onAddMissingClicked(bool /*checked*/)
{
	addTracks(getTrackIdsNotInPlaylist());
	accept();
}

void Dialog::AddToPlaylist::onAddAllClicked(bool /*checked*/)
{
	addTracks(trackIdsToAdd);
	accept();
}
