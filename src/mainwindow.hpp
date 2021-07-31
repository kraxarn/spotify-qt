#pragma once

#include "main.hpp"

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
	MainWindow(lib::settings &settings, lib::paths &paths);

	static MainWindow *find(QWidget *from);
	static auto defaultSize() -> QSize;

public:
	//region Deprecated

	/** @deprecated Use TracksList.load instead */
	bool loadAlbum(const std::string &albumId,
		const std::string &trackId = std::string());

	/** @deprecated Use SidePanel.openArtist instead */
	void openArtist(const std::string &artistId);

	/** @deprecated SidePanel.openAudioFeatures instead */
	void openAudioFeaturesWidget(const lib::spt::track &track);

	//endregion

	void setStatus(const QString &message, bool important = false);
	void status(const std::string &message, bool important = false);
	lib::spt::playback currentPlayback() const;
	void openLyrics(const lib::spt::track &track);
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
	void setCurrentLibraryItem(QTreeWidgetItem *item);
	lib::spt::playlist getPlaylist(int index);
	void setCurrentPlaylistItem(int index);
	std::unordered_set<std::string> allArtists();
	QListWidgetItem *getCurrentPlaylistItem();
	int getPlaylistItemCount();
	QListWidgetItem *getPlaylistItem(int index);
	void orderPlaylists(lib::playlist_order order);
	void setBorderless(bool enabled);

	auto startClient() -> bool;
	void stopClient();

	void setSptContext(const lib::spt::playlist &playlist);
	void setSptContext(const lib::spt::album &album);
	void setNoSptContext();

	// Getters for private properties
	void setSearchChecked(bool checked);
	TracksList *getSongsTree();
	std::string getSptContext() const;
	lib::spt::playback &getCurrentPlayback();
	const spt::Current &getCurrent();
	auto getClientHandler() -> const spt::ClientHandler *;
	void resetLibraryPlaylist() const;

#ifdef USE_DBUS
	mp::Service *getMediaPlayer();
#endif

protected:
	void closeEvent(QCloseEvent *event) override;

private:
	// Qt Widgets
	TracksList *songs = nullptr;
	MainToolBar *toolBar = nullptr;

	QSizeGrip *bottomLeftResize = nullptr;
	QSizeGrip *bottomRightResize = nullptr;

	// spt
	spt::ClientHandler *sptClient = nullptr;
	spt::Spotify *spotify = nullptr;
	spt::Current current;

	// lib
	lib::settings &settings;
	lib::paths &paths;
	lib::json_cache cache;
	lib::spt::user currentUser;
	lib::http_client *httpClient = nullptr;

	// Non-Widget Qt
	QNetworkAccessManager *network = nullptr;

	// Other
	TrayIcon *trayIcon = nullptr;
	int refreshCount = -1;
	bool stateValid = true;
	QDockWidget *sidePanel = nullptr;

	LibraryList *libraryList = nullptr;
	PlaylistList *playlistList = nullptr;
	View::Context::Context *contextView = nullptr;

#ifdef USE_DBUS
	mp::Service *mediaPlayer = nullptr;
#endif

	// Initialization
	void initClient();
	void initMediaController();
	void initWhatsNew();
	void initDevice();

	// Methods
	QWidget *createCentralWidget();
	void setAlbumImage(const std::string &url);
	void setSptContext(const std::string &uri);
};
