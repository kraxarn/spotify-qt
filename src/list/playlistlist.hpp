#pragma once

#include "../spotify/spotify.hpp"
#include "playlistlist.hpp"
#include "lib/cache.hpp"
#include "lib/spotify/playlist.hpp"
#include "lib/enum/playlistorder.hpp"

#include <QListWidget>

class PlaylistList: public QListWidget
{
Q_OBJECT

public:
	PlaylistList(spt::Spotify &spotify, lib::settings &settings, lib::cache &cache,
		QWidget *parent);

	auto getPlaylists() -> std::vector<lib::spt::playlist> &;

	void load(const std::vector<lib::spt::playlist> &items);
	void refresh();
	void order(lib::playlist_order order);

private:
	spt::Spotify &spotify;
	lib::cache &cache;
	lib::settings &settings;
	std::vector<lib::spt::playlist> playlists;

	auto getItemIndex(QListWidgetItem *item) -> int;
	void clicked(QListWidgetItem *item);
	void doubleClicked(QListWidgetItem *item);
	void menu(const QPoint &pos);

	static auto latestTrack(const std::vector<lib::spt::track> &tracks) -> int;
};