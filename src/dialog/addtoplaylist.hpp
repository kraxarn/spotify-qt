#pragma once

#include "lib/spotify/api.hpp"
#include "dialog/base.hpp"

namespace Dialog
{
	class AddToPlaylist: public Base
	{
	Q_OBJECT

	public:
		AddToPlaylist(lib::spt::api &spotify, lib::spt::playlist playlist,
			const std::vector<lib::spt::track> &playlistTracks,
			std::vector<std::string> trackIds, QWidget *parent);

		static void ask(lib::spt::api &spotify, const lib::spt::playlist &playlist,
			const std::vector<std::string> &trackIds, QWidget *parent);

	private:
		lib::spt::api &spotify;
		lib::spt::playlist playlist;
		std::unordered_set<std::string> playlistTrackIds;
		std::vector<std::string> trackIdsToAdd;

		auto shouldAsk() -> bool;

		auto getTrackIdsNotInPlaylist() -> std::vector<std::string>;

		void onAddMissingClicked(bool checked);
		void onAddAllClicked(bool checked);

		void addTracks(const std::vector<std::string> &trackIds);
	};
}
