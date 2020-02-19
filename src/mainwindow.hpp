#pragma once

#include "main.hpp"
#include "spotify/spotify.hpp"
#include "spotify/playlist.hpp"
#include "spotify/playback.hpp"

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

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(spt::Spotify *spotify, QApplication *app, QWidget *parent = nullptr);
	~MainWindow() override;
private:
	// Widgets
	QListWidget	*playlists;
	QTreeWidget	*songs;
	QLabel		*nowPlaying;
	QLabel		*position;
	QLabel		*nowAlbum;
	QSlider		*progress;
	QAction		*playPause;
	QSlider		*volume;
	QAction		*repeat;
	QAction		*shuffle;
	// Everything else
	spt::Spotify			*spotify;
	QVector<spt::Playlist>	*sptPlaylists;
	spt::Playback			current;
	QNetworkAccessManager	*network;
	// Methods
	QWidget *createCentralWidget();
	QToolBar *createToolBar();
	QMenu *createMenu();
	void refreshPlaylists();
	bool loadPlaylist(spt::Playlist &playlist);
	void setStatus(QString message);
	QStringList *getTracksAfter(QString &trackId);
	void setCurrentSongIcon();
	void setAlbumImage(QString url);
	static QString formatTime(int ms);
	void refreshDevices(QMenu *deviceMenu);
	void refresh();
};
