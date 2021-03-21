#pragma once

#include "main.hpp"

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
	MainWindow(lib::settings &settings, lib::paths &paths);

	static MainWindow *find(QWidget *from);

protected:
	void closeEvent(QCloseEvent *event) override;

public:
	QByteArray get(const QString &url);
	QJsonDocument getJson(const QString &url);
	void setStatus(const QString &message, bool important = false);
	void status(const std::string &message, bool important = false);
	bool loadAlbum(const std::string &albumId, const std::string &trackId = std::string());
	void openArtist(const std::string &artistId);
	/**
	 * @deprecated Use with url as std::string instead
	 */
	QPixmap getAlbum(const QString &url);
	QPixmap getAlbum(const std::string &url);
	QPixmap getImage(const QString &type, const QString &url);
	void refreshPlaylist(const spt::Playlist &playlist);
	bool loadPlaylist(const spt::Playlist &playlist);
	spt::Playback currentPlayback() const;
	void openAudioFeaturesWidget(const std::string &trackId,
		const std::string &artist, const std::string &name);
	void openLyrics(const std::string &artist, const std::string &name);
	std::vector<lib::spt::track> playlistTracks(const std::string &playlistId);
	void reloadTrayIcon();
	spt::User getCurrentUser();
	void setFixedWidthTime(bool value);
	std::vector<lib::spt::track> loadTracksFromCache(const std::string &id);
	void saveTracksToCache(const std::string &id, const std::vector<lib::spt::track> &tracks);
	bool loadSongs(const std::vector<lib::spt::track> &tracks,
		const std::string &selectedId = std::string());
	std::vector<std::string> currentTracks();
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
	std::unordered_set<std::string> allArtists();
	QListWidgetItem *getCurrentPlaylistItem();
	QVector<spt::Playlist> &getPlaylists();
	int getPlaylistItemCount();
	QListWidgetItem *getPlaylistItem(int index);
	void orderPlaylists(lib::playlist_order order);

	// Getters for private properties
	QString &getCacheLocation();
	QAction *getSearchAction();
	QTreeWidget *getSongsTree();
	std::string getSptContext() const;
	spt::Playback &getCurrentPlayback();

#ifdef USE_DBUS
	mp::Service *getMediaPlayer();
#endif

private:
	// Constructor
	lib::settings &settings;
	lib::cache cache;

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
	std::unordered_map<std::string, QTreeWidgetItem *> trackItems;
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
	void cachePlaylist(const spt::Playlist &playlist);
	bool loadPlaylistFromCache(const spt::Playlist &playlist);
};