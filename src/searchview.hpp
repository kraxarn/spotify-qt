#pragma once

#include "mainwindow.hpp"
#include "spotify/spotify.hpp"

#include <QDockWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTabWidget>
#include <QListWidget>

class SearchView : public QDockWidget
{
	Q_OBJECT
public:
	explicit SearchView(spt::Spotify &spotify, QWidget *parent = nullptr);

private:
	QListWidget	*trackList, *artistList, *albumList, *playlistList;
};