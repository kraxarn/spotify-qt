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
	QString currentLibraryItem();
	void reloadTrayIcon();
	spt::User getCurrentUser();
	void refreshPlaylists();
	void setFixedWidthTime(bool value);
	QVector<spt::Track> loadTracksFromCache(const QString &id);
	void saveTracksToCache(const QString &id, const QVector<spt::Track> &tracks);
	QMenu *songMenu(
		const QString &trackId, const QString &artist,
		const QString &name, const QString &artistId, const QString &albumId);
	QStringList currentTracks();
	void setPlayingTrackItem(QTreeWidgetItem *item);
	void refresh();
	bool loadSongs(const QVector<spt::Track> &tracks);
	QSet<QString> allArtists();

	// Getters for private properties
	QString &getCacheLocation();
	QVector<spt::Playlist> &getSptPlaylists();
	QListWidget *getPlaylistsList();
	QAction *getSearchAction();
	QTreeWidget *getSongsTree();
	QString &getSptContext();
	spt::Spotify *getSpotify();
	Settings &getSettings();
	spt::Playback &getCurrentPlayback();

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
	QLabel *position = nullptr;
	QSlider *progress = nullptr;
	QTreeWidgetItem *playingTrackItem = nullptr;

	// spt
	spt::ClientHandler *sptClient = nullptr;
	spt::Spotify *spotify = nullptr;
	spt::Playback current;

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
	CentralWidget *centralWidget;

	// Methods
	QToolBar *createToolBar();
	void setAlbumImage(const QString &url);
	void refreshed(const spt::Playback &playback);
	void cachePlaylist(spt::Playlist &playlist);
	bool loadPlaylistFromCache(spt::Playlist &playlist);
};