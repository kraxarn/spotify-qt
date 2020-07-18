#pragma once

#include "main.hpp"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(Settings &settings);

public:
	static void applyPalette(Settings::Palette palette);
	static bool hasDarkBackground();
	// Constants for track info
	enum DataRole {
		RoleTrackId		= 0x0100,	// 256
		RoleArtistId	= 0x0101,	// 257
		RoleAlbumId		= 0x0102,	// 258
		RoleIndex		= 0x0103,	// 259
		RolePlaylistId	= 0x0104	// 260
	};
	enum class MaskShape {
		App,
		Pie
	};
	QByteArray get(const QString &url);
	QJsonDocument getJson(const QString &url);
	void setStatus(const QString &message, bool important = false);
	bool loadAlbum(const QString &albumId, bool ignoreEmpty = true);
	void openArtist(const QString &artistId);
	static QWidget *layoutToWidget(QLayout *layout);
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
	static QPixmap mask(const QPixmap &source, MaskShape shape = MaskShape::App, const QVariant &data = QVariant());
	QVector<spt::Track> loadTracksFromCache(const QString &id);
	void saveTracksToCache(const QString &id, const QVector<spt::Track> &tracks);

	// Getters for private properties
	QString &getCacheLocation();
	QVector<spt::Playlist> &getSptPlaylists();
	QListWidget *getPlaylistsList();
	QAction *getSearchAction();
	QTreeWidget *getSongsTree();
	QString &getSptContext();

private:
	// Static
	static bool darkBackground;

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
	QTreeWidget *libraryList = nullptr;
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
	bool loadSongs(const QVector<spt::Track> &tracks);
	void setAlbumImage(const QString &url);
	static QString formatTime(int ms);
	void refresh();
	void refreshed(const spt::Playback &playback);
	void cachePlaylist(spt::Playlist &playlist);
	bool loadPlaylistFromCache(spt::Playlist &playlist);
	QMenu *songMenu(const QString &trackId, const QString &artist,
		const QString &name, const QString &artistId, const QString &albumId);
	QTreeWidgetItem *treeItem(QTreeWidget *tree, const QString &name, const QString &toolTip = QString::Null(),
		const QStringList &childrenItems = QStringList(QString::Null()));
	QStringList currentTracks();
	void setPlayingTrackItem(QTreeWidgetItem *item);
	QSet<QString> allArtists();
};