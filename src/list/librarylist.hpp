#pragma once

class LibraryList;

#include "../enum/datarole.hpp"
#include "../mainwindow.hpp"
#include "../spotify/spotify.hpp"
#include "../util/utils.hpp"

#include <QTreeWidget>
#include <QHeaderView>

class LibraryList: public QTreeWidget
{
Q_OBJECT

public:
	LibraryList(spt::Spotify &spotify, QWidget *parent);

private:
	spt::Spotify &spotify;
	QWidget *parent = nullptr;

	void clicked(QTreeWidgetItem *item, int column);
	void doubleClicked(QTreeWidgetItem *item, int column);
	void expanded(QTreeWidgetItem *item);
};


