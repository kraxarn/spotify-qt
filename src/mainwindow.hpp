#pragma once

#include "settingsdialog.hpp"
#include "spotify/spotify.hpp"
#include "spotify/playlist.hpp"
#include "spotify/playback.hpp"
#include "spotify/webplayer.hpp"
#include "spotify/audiofeatures.hpp"
#include "spotify/clienthandler.hpp"

#include <QMainWindow>
#include <QListWidget>
#include <QTreeWidget>
#include <QLabel>
#include <QSlider>
#include <QAction>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QGroupBox>
#include <QIcon>
#include <QHeaderView>
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>
#include <QMenu>
#include <QMessageBox>
#include <QDockWidget>
#include <QTextEdit>
#include <QApplication>
#include <QMapIterator>
#include <QClipboard>
#include <QFileInfo>

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;
private:
	// Widgets
	QListWidget	*playlists;
	QTreeWidget	*songs;
	QLabel		*nowPlaying, *position, *nowAlbum;
	QSlider		*progress, *volume;
	QAction		*playPause, *repeat, *shuffle;
	// Everything else
	spt::Spotify			*spotify;
	QVector<spt::Playlist>	*sptPlaylists;
	spt::Playback			current;
	QNetworkAccessManager	*network;
	spt::ClientHandler		*sptClient;
	QString					cacheLocation;
	// Constants for track info
	enum DataRole {
		RoleTrackId		= 0x0100,	// 256
		RoleArtistId	= 0x0101,	// 257
		RoleAlbumId		= 0x0102,	// 258
		RoleIndex		= 0x0103	// 259
	};
	// What Spotify context we're currently in
	QString sptContext;
	// Methods
	QWidget *createCentralWidget();
	QToolBar *createToolBar();
	QMenu *createMenu();
	QAction *createMenuAction(const QString &iconName, const QString &text, QKeySequence::StandardKey shortcut);
	void refreshPlaylists();
	bool loadSongs(const QVector<spt::Track> &tracks);
	bool loadAlbum(const QString &albumId, bool ignoreEmpty = true);
	bool loadPlaylist(spt::Playlist &playlist);
	void setStatus(const QString &message);
	void setCurrentSongIcon();
	void setAlbumImage(const QString &url);
	static QString formatTime(int ms);
	void refreshDevices(QMenu *deviceMenu);
	void refresh();
	QByteArray get(const QString &url);
	QJsonDocument getJson(const QString &url);
	QPixmap getAlbum(const QString &url);
	void openAudioFeaturesWidget(const QString &trackId, const QString &artist, const QString &name);
	void openLyrics(const QString &artist, const QString &name);
	void openArtist(const QString &artistId);
	void cachePlaylist(spt::Playlist &playlist);
	bool loadPlaylistFromCache(spt::Playlist &playlist);
	QVector<spt::Track> playlistTracks(const QString &playlistId);
	void refreshPlaylist(spt::Playlist &playlist, bool force = false);
	QMenu *songMenu(QWidget *parent, const QString &trackId, const QString &artist,
		const QString &name, const QString &artistId, const QString &albumId);
	QIcon icon(const QString &name);
};
