#pragma once

#include "list/librarylist.hpp"
#include "list/playlistlist.hpp"
#include "spotify/current.hpp"
#include "spotify/spotify.hpp"
#include "util/utils.hpp"
#include "lib/developermode.hpp"
#include "util/dateutils.hpp"
#include "lib/cache.hpp"

#include <unordered_set>

#include <QVBoxLayout>

class LeftSidePanel: public QWidget
{
Q_OBJECT

public:
	explicit LeftSidePanel(spt::Spotify &spotify, lib::settings &settings,
		spt::Current &current, lib::cache &cache, QWidget *parent);

	auto allArtists() -> std::unordered_set<std::string>;

	void updateContextIcon();

	auto getCurrentlyPlaying() const -> const lib::spt::track &;

	void setCurrentlyPlaying(const lib::spt::track &value);

	void setAlbumImage(const QPixmap &pixmap);

	/**
	 * Shows default album icon and "No music playing"
	 */
	void resetCurrentlyPlaying();

	//region Playlists

	auto playlistItem(int index) const -> QListWidgetItem *;

	auto playlistItemCount() const -> int;

	void setCurrentPlaylistItem(int index) const;

	auto currentPlaylist() -> QListWidgetItem *;

	void refreshPlaylists();

	auto playlist(size_t index) -> lib::spt::playlist &;

	auto getPlaylistNameFromSaved(const std::string &id) -> std::string;

	auto getPlaylists() -> std::vector<lib::spt::playlist> &;

	void orderPlaylists(lib::playlist_order order);

	void setCurrentPlaylistItem(QListWidgetItem *item);

	//endregion

	//region Library

	void setCurrentLibraryItem(QTreeWidgetItem *item);

	//endregion

private:
	spt::Spotify &spotify;
	spt::Current &current;
	lib::settings &settings;
	LibraryList *libraryList;
	PlaylistList *playlists;
	lib::cache &cache;
	lib::spt::track currentlyPlaying;

	QLabel *contextIcon = nullptr;
	QLabel *contextInfo = nullptr;
	QLabel *nowAlbum = nullptr;
	QLabel *nowPlaying = nullptr;

	void contextInfoMenu(const QPoint &pos);

	void contextInfoOpen(bool checked);

	auto currentContextIcon() const -> QIcon;

	void getPlaylistName(const std::string &id, lib::callback<std::string> &callback);

	void popupSongMenu(const QPoint &pos);
};
