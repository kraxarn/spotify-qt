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

	std::unordered_set<std::string> allArtists();
	void updateContextIcon();

	QString getCurrentlyPlaying();
	void setCurrentlyPlaying(const QString &value);

	void setAlbumImage(const QPixmap &pixmap);

	//region Playlists

	QListWidgetItem *playlistItem(int index) const;
	int playlistItemCount() const;
	void setCurrentPlaylistItem(int index) const;
	QListWidgetItem *currentPlaylist();
	void refreshPlaylists();
	int playlistCount() const;
	lib::spt::playlist &playlist(size_t index);
	std::string getPlaylistNameFromSaved(const std::string &id);
	std::vector<lib::spt::playlist> &getPlaylists();
	void orderPlaylists(lib::playlist_order order);

	//endregion

	//region Library

	void setCurrentLibraryItem(QTreeWidgetItem *item);
	QTreeWidgetItem *getCurrentLibraryItem();

	//endregion

private:
	spt::Spotify &spotify;
	spt::Current &current;
	lib::settings &settings;
	LibraryList *libraryList;
	PlaylistList *playlists;
	lib::cache &cache;

	QLabel *contextIcon = nullptr;
	QLabel *contextInfo = nullptr;
	QLabel *nowAlbum = nullptr;
	QLabel *nowPlaying = nullptr;

	void contextInfoMenu(const QPoint &pos);
	void contextInfoOpen(bool checked);
	QIcon currentContextIcon() const;
	std::string getPlaylistName(const std::string &id);

	void popupSongMenu(const QPoint &pos);
};
