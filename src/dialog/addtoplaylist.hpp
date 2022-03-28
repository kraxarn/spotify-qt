#pragma once

#include "lib/spotify/api.hpp"
#include "dialog/base.hpp"

namespace Dialog
{
	class AddToPlaylist: public Base
	{
	public:
		AddToPlaylist(lib::spt::api &spotify, lib::spt::playlist playlist,
			std::vector<std::string> trackIds, QWidget *parent);

	protected:
		void showEvent(QShowEvent *playlistTracks) override;

	private:
		lib::spt::api &spotify;
		lib::spt::playlist playlist;
		std::unordered_set<std::string> playlistTrackIds;
		std::vector<std::string> trackIdsToAdd;

		auto shouldAsk() -> bool;
		void ask();

		auto getTrackIdsNotInPlaylist() -> std::vector<std::string>;

		void onAddMissingClicked(bool checked);
		void onAddAllClicked(bool checked);

		void addTracks(const std::vector<std::string> &trackIds);
	};
}
