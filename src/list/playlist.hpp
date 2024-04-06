#pragma once

#include "lib/cache.hpp"
#include "lib/spotify/api.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/enum/playlistorder.hpp"
#include "util/tooltip.hpp"

#include <QListWidget>

namespace List
{
	class Playlist: public QListWidget
	{
	Q_OBJECT

	public:
		Playlist(lib::spt::api &spotify, lib::settings &settings,
			lib::cache &cache, const lib::http_client &httpClient, QWidget *parent);

		void refresh();
		void order(lib::playlist_order item1);

		auto allArtists() -> std::unordered_set<std::string>;

		auto at(int index) const -> lib::spt::playlist;

	protected:
		void showEvent(QShowEvent *event) override;

	private:
		lib::spt::api &spotify;
		lib::cache &cache;
		lib::settings &settings;
		Tooltip tooltip;

		void load(const std::vector<lib::spt::playlist> &playlists, int offset);
		void selectActive();

		auto getItemIndex(QListWidgetItem *item) -> int;
		auto getPlaylists() const -> std::vector<lib::spt::playlist>;

		void onItemClicked(QListWidgetItem *item);
		void onItemDoubleClicked(QListWidgetItem *item);
		void onContextMenuRequested(const QPoint &pos);
		void onItemEntered(QListWidgetItem *item);

		static auto latestTrack(const std::vector<lib::spt::track> &tracks) -> QDateTime;
	};
}
