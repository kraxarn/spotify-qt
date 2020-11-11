#pragma once

#include "main.hpp"

class MainWindow: public QMainWindow
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
	QVector<spt::Track> playlistTracks(const QString &playlistId);
	void reloadTrayIcon();
	spt::User getCurrentUser();
	void refreshPlaylists();
	void setFixedWidthTime(bool value);
	QVector<spt::Track> loadTracksFromCache(const QString &id);
	void saveTracksToCache(const QString &id, const QVector<spt::Track> &tracks);
	bool loadSongs(const QVector<spt::Track> &tracks);
	QSet<QString> allArtists();
	QStringList currentTracks();
	void setPlayingTrackItem(QTreeWidgetItem *item);
	void refresh();
	void refreshed(const spt::Playback &playback);
	void orderPlaylists(PlaylistOrder order);
	void toggleTrackNumbers(bool enabled);
	bool isValid() const;

	// Getters for private properties
	QString &getCacheLocation();
	QVector<spt::Playlist> &getSptPlaylists();
	QListWidget *getPlaylistsList();
	QAction *getSearchAction();
	QTreeWidget *getSongsTree();
	LibraryList *getLibraryList();
	QString &getSptContext();
	spt::Playback &getCurrentPlayback();

#ifdef USE_DBUS
	mp::Service *getMediaPlayer();
#endif

private:
	// Constructor
	Settings &settings;

	// Qt Widgets
	QLabel *nowAlbum = nullptr;
	QLabel *nowPlaying = nullptr;
	QListWidget *playlists = nullptr;
	LibraryList *libraryList = nullptr;
	QTreeWidget *songs = nullptr;
	QTreeWidgetItem *playingTrackItem = nullptr;
	QToolBar *toolBar = nullptr;
	QLabel *contextIcon = nullptr;
	QLabel *contextInfo = nullptr;

	// spt
	spt::ClientHandler *sptClient = nullptr;
	spt::Playback current;
	spt::Spotify *spotify = nullptr;

	// Non-Widget Qt
	QNetworkAccessManager *network = nullptr;

	// Other
	AudioFeaturesView *audioFeaturesView = nullptr;
	LyricsView *lyricsView = nullptr;
	QHash<QString, QTreeWidgetItem *> trackItems;
	QString cacheLocation;
	QString sptContext;
	QVector<spt::Playlist> sptPlaylists;
	TrayIcon *trayIcon = nullptr;
	int refreshCount = -1;
	spt::User currentUser;
	QIcon emptyIcon;
	bool stateValid = true;
	QWidget *sidePanel = nullptr;

#ifdef USE_DBUS
	mp::Service *mediaPlayer = nullptr;
#endif

	// Methods
	QWidget *createCentralWidget();
	void setAlbumImage(const QString &url);
	void cachePlaylist(spt::Playlist &playlist);
	bool loadPlaylistFromCache(spt::Playlist &playlist);
	void updateContextIcon();
	QString getPlaylistName(const QString &id);
	QString getPlaylistNameFromSaved(const QString &id);
	void contextInfoMenu(const QPoint &pos);
	void contextInfoOpen(bool checked);
	QIcon currentContextIcon() const;
};