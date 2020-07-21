#pragma once

#include "main.hpp"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(Settings &settings);

protected:
	void closeEvent(QCloseEvent *event) override;

public:
	QByteArray get(const QString &url);
	QJsonDocument getJson(const QString &url);
	void setStatus(const QString &message, bool important = false);
	bool loadAlbum(const QString &albumId, bool ignoreEmpty = true);
	void openArtist(const QString &artistId);
	QPixmap getAlbum(const QString &url);
	QPixmap getImage(const QString &type, const QString &url);
	void refreshPlaylist(spt::Playlist &playlist);
	bool loadPlaylist(spt::Playlist &playlist);
	spt::Playback currentPlayback();
	void openAudioFeaturesWidget(const QString &trackId, const QString &artist, const QString &name);
	void openLyrics(const QString &artist, const QString &name);
	bool hasPlaylistSelected();
	QVector<spt::Track> playlistTracks(const QString &playlistId);
	void reloadTrayIcon();
	spt::User getCurrentUser();
	void refreshPlaylists();
	void setFixedWidthTime(bool value);
	QVector<spt::Track> loadTracksFromCache(const QString &id);
	void saveTracksToCache(const QString &id, const QVector<spt::Track> &tracks);
	bool loadSongs(const QVector<spt::Track> &tracks);
	QSet<QString> allArtists();
	QMenu *songMenu(
		const QString &trackId, const QString &artist,
		const QString &name, const QString &artistId, const QString &albumId);
	QStringList currentTracks();
	void setPlayingTrackItem(QTreeWidgetItem *item);
	void refresh();

	// Getters for private properties
	QString &getCacheLocation();
	QVector<spt::Playlist> &getSptPlaylists();
	QListWidget *getPlaylistsList();
	QAction *getSearchAction();
	QTreeWidget *getSongsTree();
	LibraryList *getLibraryList();
	QString &getSptContext();

private:
	// Constructor
	Settings &settings;

	// Qt Widgets
	QAction *playPause = nullptr;
	QAction *repeat = nullptr;
	QAction *search = nullptr;
	QAction *shuffle = nullptr;
	QDockWidget *artistView = nullptr;
	QDockWidget *searchView = nullptr;
	QLabel *nowAlbum = nullptr;
	QLabel *nowPlaying = nullptr;
	QLabel *position = nullptr;
	QListWidget *playlists = nullptr;
	QSlider *progress = nullptr;
	LibraryList *libraryList = nullptr;
	QTreeWidget *songs = nullptr;
	QTreeWidgetItem *playingTrackItem = nullptr;

	// spt
	spt::ClientHandler *sptClient = nullptr;
	spt::Playback current;
	spt::Spotify *spotify = nullptr;

	// Non-Widget Qt
	QNetworkAccessManager *network = nullptr;

	// Other
	AudioFeaturesView *audioFeaturesView = nullptr;
	LyricsView *lyricsView = nullptr;
	QHash<QString, QTreeWidgetItem*> trackItems;
	QString cacheLocation;
	QString sptContext;
	QVector<spt::Playlist> sptPlaylists;
	TrayIcon *trayIcon = nullptr;
	VolumeButton *volumeButton = nullptr;
	int refreshCount = -1;
	mp::Service *mediaPlayer = nullptr;
	spt::User currentUser;

	// Methods
	QWidget *createCentralWidget();
	QToolBar *createToolBar();
	void setAlbumImage(const QString &url);
	void refreshed(const spt::Playback &playback);
	void cachePlaylist(spt::Playlist &playlist);
	bool loadPlaylistFromCache(spt::Playlist &playlist);
};