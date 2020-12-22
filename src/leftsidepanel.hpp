#pragma once

#include "list/librarylist.hpp"
#include "list/playlistlist.hpp"
#include "spotify/current.hpp"
#include "spotify/spotify.hpp"
#include "util/utils.hpp"

#include <QVBoxLayout>

class LeftSidePanel: public QWidget
{
Q_OBJECT

public:
	explicit LeftSidePanel(spt::Spotify &spotify, Settings &settings, spt::Current &current, QWidget *parent = nullptr);

	QSet<QString> allArtists();
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
	spt::Playlist &playlist(size_t index);
	QString getPlaylistNameFromSaved(const QString &id);
	QVector<spt::Playlist> &getPlaylists();
	void orderPlaylists(PlaylistOrder order);

	//endregion

	//region Library

	void setCurrentLibraryItem(QTreeWidgetItem *item);
	QTreeWidgetItem *getCurrentLibraryItem();


	//endregion

private:
	spt::Spotify &spotify;
	spt::Current &current;
	Settings &settings;
	LibraryList *libraryList;
	PlaylistList *playlists;
	QVector<spt::Playlist> sptPlaylists;

	QLabel *contextIcon = nullptr;
	QLabel *contextInfo = nullptr;
	QLabel *nowAlbum = nullptr;
	QLabel *nowPlaying = nullptr;

	static int latestTrack(const QVector<spt::Track> &tracks);
	void contextInfoMenu(const QPoint &pos);
	void contextInfoOpen(bool checked);
	QIcon currentContextIcon() const;

	QString getPlaylistName(const QString &id);
};
