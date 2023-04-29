#pragma once

#include "main.hpp"

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
	MainWindow(lib::settings &settings, lib::paths &paths,
		lib::qt::http_client &httpClient, lib::spt::api &spotify);

	static auto find(QObject *from) -> MainWindow *;
	static auto defaultSize() -> QSize;

	//region Deprecated

	/** @deprecated Use TracksList.load instead */
	void loadAlbum(const std::string &albumId, const std::string &trackId = std::string());

	/** @deprecated Use SidePanel.openArtist instead */
	void openArtist(const std::string &artistId);

	//endregion

	auto playback() const -> const lib::spt::playback &;
	void openLyrics(const lib::spt::track &track);
	void reloadTrayIcon();
	auto getTrayIcon() -> TrayIcon *;
	auto getCurrentUser() const -> const lib::spt::user &;
	std::vector<lib::spt::track> loadTracksFromCache(const std::string &id);
	void saveTracksToCache(const std::string &id, const std::vector<lib::spt::track> &tracks);
	std::vector<std::string> currentTracks();
	void refresh();
	void refreshed(const lib::spt::playback &playback);
	void toggleTrackNumbers(bool enabled);
	void toggleExpandableAlbum(lib::album_size albumSize);
	void setSearchVisible(bool visible);
	void refreshPlaylists();
	void setCurrentLibraryItem(QTreeWidgetItem *item);
	lib::spt::playlist getPlaylist(int index);
	void setCurrentPlaylistItem(int index);
	std::unordered_set<std::string> allArtists();
	QListWidgetItem *getCurrentPlaylistItem();
	int getPlaylistItemCount();
	QListWidgetItem *getPlaylistItem(int index);
	void orderPlaylists(lib::playlist_order order);
	void setBorderless(bool enabled);
	void addSizeGrip(const std::function<QPoint(const QRect &)> &position);
	void minimize();

	auto startClient() -> const SpotifyClient::Runner *;
	void stopClient();

	// Getters for private properties
	void setSearchChecked(bool checked);
	List::Tracks *getSongsTree();
	const spt::Current &getCurrent();
	auto getSpotifyRunner() -> const SpotifyClient::Runner *;
	void resetLibraryPlaylist() const;
	auto history() -> HistoryButton *;

#ifdef USE_DBUS
	mp::Service *getMediaPlayer();
#endif

#ifdef _WIN32
	void registerMediaHotkeys(bool enabled);
#endif

signals:
	void playbackRefreshed(const lib::spt::playback &playback);

protected:
	void closeEvent(QCloseEvent *event) override;
	auto eventFilter(QObject *obj, QEvent *event) -> bool override;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	bool nativeEvent(const QByteArray &eventType, void *message, qintptr *result) override;
#else
	bool nativeEvent(const QByteArray &eventType, void *message, long *result) override;
#endif

private:
	MainContent *mainContent = nullptr;
	MainToolBar *toolBar = nullptr;
	QList<QSizeGrip *> resizeGrips;

	SpotifyClient::Runner *spotifyRunner = nullptr;

	lib::spt::api &spotify;
	spt::Current current;

	lib::settings &settings;
	lib::paths &paths;
	lib::json_cache cache;
	lib::spt::user currentUser;
	lib::http_client &httpClient;

	TrayIcon *trayIcon = nullptr;
	int refreshCount = -1;
	QDockWidget *sidePanel = nullptr;

	List::Library *libraryList = nullptr;
	List::Playlist *playlistList = nullptr;
	Context::View *contextView = nullptr;

#ifdef USE_DBUS
	mp::Service *mediaPlayer = nullptr;
#endif

	// Initialization
	void initClient();
	void initMediaController();
	void initMediaHotkeys();
	void initWhatsNew();
	void initDevice();
	void checkForUpdates();

	// Methods
	QWidget *createCentralWidget();
	void setAlbumImage(const lib::spt::entity &albumEntity, const std::string &albumImageUrl);

	// Slots
	void onSpotifyStatusChanged(const QString &status);
};
