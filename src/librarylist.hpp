#pragma once

class LibraryList;

#include "utils.hpp"
#include "mainwindow.hpp"
#include "spotify/spotify.hpp"

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

	void clicked(QTreeWidgetItem *item);
	void doubleClicked(QTreeWidgetItem *item);
	void expanded(QTreeWidgetItem *item);
};


