#pragma once

#include "main.hpp"

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
	explicit MainWindow(lib::settings &settings);

	static MainWindow *find(QWidget *from);

protected:
	void closeEvent(QCloseEvent *event) override;

public:
	QByteArray get(const QString &url);
	QJsonDocument getJson(const QString &url);
	void setStatus(const QString &message, bool important = false);
	bool loadAlbum(const QString &albumId, const QString &trackId = QString());
	void openArtist(const QString &artistId);
	QPixmap getAlbum(const QString &url);
	QPixmap getImage(const QString &type, const QString &url);
	void refreshPlaylist(spt::Playlist &playlist);
	bool loadPlaylist(spt::Playlist &playlist);
	spt::Playback currentPlayback() const;
	void openAudioFeaturesWidget(const QString &trackId, const QString &artist, const QString &name);
	void openLyrics(const QString &artist, const QString &name);
	QVector<spt::Track> playlistTracks(const QString &playlistId);
	void reloadTrayIcon();
	spt::User getCurrentUser();
	void setFixedWidthTime(bool value);
	QVector<spt::Track> loadTracksFromCache(const QString &id);
	void saveTracksToCache(const QString &id, const QVector<spt::Track> &tracks);
	bool loadSongs(const QVector<spt::Track> &tracks, const QString &selectedId = QString());
	QStringList currentTracks();
	void setPlayingTrackItem(QTreeWidgetItem *item);
	void refresh();
	void refreshed(const spt::Playback &playback);
	void toggleTrackNumbers(bool enabled);
	bool isValid() const;
	void setSearchVisible(bool visible);
	void addSidePanelTab(QWidget *widget, const QString &title);
	void refreshPlaylists();
	QTreeWidgetItem *getCurrentLibraryItem();
	void setCurrentLibraryItem(QTreeWidgetItem *item);
	spt::Playlist &getPlaylist(int index);
	void setCurrentPlaylistItem(int index);
	QSet<QString> allArtists();
	QListWidgetItem *getCurrentPlaylistItem();
	QVector<spt::Playlist> &getPlaylists();
	int getPlaylistItemCount();
	QListWidgetItem *getPlaylistItem(int index);
	void orderPlaylists(lib::playlist_order order);

	// Getters for private properties
	QString &getCacheLocation();
	QAction *getSearchAction();
	QTreeWidget *getSongsTree();
	QString &getSptContext();
	spt::Playback &getCurrentPlayback();

#ifdef USE_DBUS
	mp::Service *getMediaPlayer();
#endif

private:
	// Constructor
	lib::settings &settings;

	// Qt Widgets
	QTreeWidget *songs = nullptr;
	QTreeWidgetItem *playingTrackItem = nullptr;
	QToolBar *toolBar = nullptr;

	// spt
	spt::ClientHandler *sptClient = nullptr;
	spt::Spotify *spotify = nullptr;
	spt::Current current;

	// Non-Widget Qt
	QNetworkAccessManager *network = nullptr;

	// Other
	LyricsView *lyricsView = nullptr;
	QHash<QString, QTreeWidgetItem *> trackItems;
	QString cacheLocation;
	TrayIcon *trayIcon = nullptr;
	int refreshCount = -1;
	spt::User currentUser;
	QIcon emptyIcon;
	bool stateValid = true;
	QTabWidget *sidePanel = nullptr;
	LeftSidePanel *leftSidePanel = nullptr;

#ifdef USE_DBUS
	mp::Service *mediaPlayer = nullptr;
#endif

	// Methods
	QWidget *createCentralWidget();
	void setAlbumImage(const QString &url);
	void cachePlaylist(spt::Playlist &playlist);
	bool loadPlaylistFromCache(spt::Playlist &playlist);
};