#pragma once

#include "albummenu.hpp"
#include "mainwindow.hpp"
#include "songmenu.hpp"
#include "spotify/spotify.hpp"

#include <QDockWidget>
#include <QLabel>
#include <QListWidget>
#include <QTabWidget>
#include <QToolTip>
#include <QTreeWidget>
#include <QVBoxLayout>

class ArtistView : public QDockWidget
{
	Q_OBJECT

public:
	ArtistView(spt::Spotify &spotify, const QString &artistId, QWidget *parent);

private:
	QListWidget *relatedList = nullptr;
	QListWidget *topTracksList = nullptr;
	QPushButton *followButton = nullptr;
	QStringList topTrackIds;
	QWidget *parent = nullptr;
	const QString &artistId;
	spt::Artist artist;
	spt::Spotify &spotify;

	void follow(bool checked);
	void trackClick(QListWidgetItem *item);
	void trackMenu(const QPoint &pos);
	void loadAlbumId(QTreeWidgetItem *item);
	void relatedClick(QListWidgetItem *item);
	void albumMenu(const QPoint &pos);
};