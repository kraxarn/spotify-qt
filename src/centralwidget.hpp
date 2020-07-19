#pragma once

#include "mainwindow.hpp"
#include "spotify/spotify.hpp"

#include <QWidget>

class CentralWidget : public QSplitter
{
	Q_OBJECT

public:
	explicit CentralWidget(spt::Spotify &spotify, QWidget *parent);

	void refreshPlaylists();

	QTreeWidget *libraryList = nullptr;
	QListWidget *playlists = nullptr;
	MainWindow *mainWindow = nullptr;
	QTreeWidget *songs = nullptr;
	QLabel *nowAlbum = nullptr;
	QLabel *nowPlaying = nullptr;

private:
	spt::Spotify &spotify;
};