#pragma once

#include "mainwindow.hpp"
#include "spotify/spotify.hpp"

#include <QDockWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTabWidget>
#include <QListWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class SearchView : public QDockWidget
{
	Q_OBJECT
public:
	explicit SearchView(spt::Spotify &spotify, QWidget *parent = nullptr);

private:
	QTreeWidget	*trackList, *albumList;
	QListWidget	*artistList, *playlistList;

	QTreeWidget *defaultTree(const QStringList &headers);
};