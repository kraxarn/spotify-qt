#pragma once

#include "spotify/spotify.hpp"
#include "util/utils.hpp"
#include "listitem/libraryartistitem.hpp"
#include "listitem/libraryalbumitem.hpp"

#include <QTreeWidget>
#include <QHeaderView>

class LibraryList: public QTreeWidget
{
Q_OBJECT

public:
	LibraryList(spt::Spotify &spotify, QWidget *parent);

private:
	spt::Spotify &spotify;

	void clicked(QTreeWidgetItem *item, int column);
	void doubleClicked(QTreeWidgetItem *item, int column);
	void expanded(QTreeWidgetItem *item);

	void tracksLoaded(const std::string &id, const std::vector<lib::spt::track> &tracks);
	//static void itemsLoaded(std::vector<LibraryItem> &items, QTreeWidgetItem *item);
};
