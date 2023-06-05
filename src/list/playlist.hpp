#pragma once

#include "lib/cache.hpp"
#include "lib/spotify/api.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/enum/playlistorder.hpp"

#include <QListWidget>

namespace List
{
	class Playlist: public QListWidget
	{
	Q_OBJECT

	public:
		Playlist(lib::spt::api &spotify, lib::settings &settings,
			lib::cache &cache, QWidget *parent);

		void load(const std::vector<lib::spt::playlist> &items);
		void refresh();
		void order(lib::playlist_order item1);

		auto allArtists() -> std::unordered_set<std::string>;

		auto at(int index) -> lib::spt::playlist;
		auto at(const std::string &playlistId) -> lib::spt::playlist;

	protected:
		void showEvent(QShowEvent *event) override;

	private:
		lib::spt::api &spotify;
		lib::cache &cache;
		lib::settings &settings;

		auto getItemIndex(QListWidgetItem *item) -> int;
		void clicked(QListWidgetItem *item);
		void doubleClicked(QListWidgetItem *item);
		void menu(const QPoint &pos);

		static auto latestTrack(const std::vector<lib::spt::track> &tracks) -> QDateTime;
	};
}
