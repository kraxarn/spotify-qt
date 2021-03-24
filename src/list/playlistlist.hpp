#pragma once

#include "../spotify/spotify.hpp"
#include "playlistlist.hpp"
#include "lib/cache.hpp"

#include <QListWidget>

class PlaylistList: public QListWidget
{
Q_OBJECT

public:
	PlaylistList(spt::Spotify &spotify, lib::cache &cache, QWidget *parent);

private:
	spt::Spotify &spotify;
	lib::cache &cache;

	int getItemIndex(QListWidgetItem *item);
	void clicked(QListWidgetItem *item);
	void doubleClicked(QListWidgetItem *item);
	void menu(const QPoint &pos);
};