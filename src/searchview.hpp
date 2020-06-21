#pragma once

#include "mainwindow.hpp"
#include "spotify/spotify.hpp"

#include <QDockWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

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