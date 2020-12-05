#pragma once

#include "../mainwindow.hpp"
#include "playlistlist.hpp"
#include "spotify/spotify.hpp"

#include <QListWidget>

class PlaylistList: public QListWidget
{
Q_OBJECT

public:
	PlaylistList(spt::Spotify &spotify, QWidget *parent);

private:
	spt::Spotify &spotify;
	QWidget *parent;

	int getItemIndex(QListWidgetItem *item);
	void clicked(QListWidgetItem *item);
	void doubleClicked(QListWidgetItem *item);
	void menu(const QPoint &pos);
};