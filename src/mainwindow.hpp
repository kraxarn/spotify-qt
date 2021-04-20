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
	std::vector<unsigned char> get(const std::string &url);
	QJsonDocument getJson(const QString &url);
	void setStatus(const QString &message, bool important = false);
	void status(const std::string &message, bool important = false);
	bool loadAlbum(const std::string &albumId, const std::string &trackId = std::string());
	void openArtist(const std::string &artistId);
	QPixmap getAlbum(const std::string &url);
	lib::spt::playback currentPlayback() const;
	void openAudioFeaturesWidget(const std::string &trackId,
		const std::string &artist, const std::string &name);
	void openLyrics(const std::string &artist, const std::string &name);
	void reloadTrayIcon();
	lib::spt::user getCurrentUser();
	void setFixedWidthTime(bool value);
	std::vector<lib::spt::track> loadTracksFromCache(const std::string &id);
	void saveTracksToCache(const std::string &id, const std::vector<lib::spt::track> &tracks);
	std::vector<std::string> currentTracks();
	void refresh();
	void refreshed(const lib::spt::playback &playback);
	void toggleTrackNumbers(bool enabled);
	bool isValid() const;
	void setSearchVisible(bool visible);
	void addSidePanelTab(QWidget *widget, const QString &title);
	void refreshPlaylists();
	QTreeWidgetItem *getCurrentLibraryItem();
	void setCurrentLibraryItem(QTreeWidgetItem *item);
	lib::spt::playlist &getPlaylist(int index);
	void setCurrentPlaylistItem(int index);
	std::unordered_set<std::string> allArtists();
	QListWidgetItem *getCurrentPlaylistItem();
	std::vector<lib::spt::playlist> &getPlaylists();
	int getPlaylistItemCount();
	QListWidgetItem *getPlaylistItem(int index);
	void orderPlaylists(lib::playlist_order order);
	void startClient();

	void setSptContext(const std::string &uri);
	void setSptContext(const lib::spt::playlist &playlist);
	void setSptContext(const lib::spt::album &album);

	// Getters for private properties
	QString &getCacheLocation();
	QAction *getSearchAction();
	TracksList *getSongsTree();
	std::string getSptContext() const;
	lib::spt::playback &getCurrentPlayback();
	const spt::Current &getCurrent();

#ifdef USE_DBUS
	mp::Service *getMediaPlayer();
#endif

private:
	// Constructor
	lib::settings &settings;
	lib::cache cache;

	// Qt Widgets
	TracksList *songs = nullptr;
	QToolBar *toolBar = nullptr;

	// spt
	spt::ClientHandler *sptClient = nullptr;
	spt::Spotify *spotify = nullptr;
	spt::Current current;

	// Non-Widget Qt
	QNetworkAccessManager *network = nullptr;

	// Other
	LyricsView *lyricsView = nullptr;
	QString cacheLocation;
	TrayIcon *trayIcon = nullptr;
	int refreshCount = -1;
	lib::spt::user currentUser;
	bool stateValid = true;
	QTabWidget *sidePanel = nullptr;
	LeftSidePanel *leftSidePanel = nullptr;

#ifdef USE_DBUS
	mp::Service *mediaPlayer = nullptr;
#endif

	// Methods
	QWidget *createCentralWidget();
	void setAlbumImage(const std::string &url);
};