#pragma once

#include "../mainwindow.hpp"
#include "spotify/spotify.hpp"

#include <QDockWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QTabWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

class SearchView: public QWidget
{
Q_OBJECT

public:
	explicit SearchView(spt::Spotify &spotify, const Settings &settings, QWidget *parent = nullptr);

private:
	QListWidget *artistList = nullptr;
	QListWidget *playlistList = nullptr;
	QTreeWidget *albumList = nullptr;
	QTreeWidget *trackList = nullptr;
	QWidget *parent = nullptr;
	spt::Spotify &spotify;

	QTreeWidget *defaultTree(const QStringList &headers);
	void albumMenu(const QPoint &pos);

protected:
	void showEvent(QShowEvent *event) override;
	void hideEvent(QHideEvent *event) override;
};