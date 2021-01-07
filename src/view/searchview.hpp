#pragma once

#include "../mainwindow.hpp"
#include "../spotify/spotify.hpp"

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
	explicit SearchView(spt::Spotify &spotify, const lib::settings &settings, QWidget *parent);

private:
	QTabWidget *tabs = nullptr;
	QLineEdit *searchBox = nullptr;
	QListWidget *artistList = nullptr;
	QListWidget *playlistList = nullptr;
	QTreeWidget *albumList = nullptr;
	QTreeWidget *trackList = nullptr;
	spt::Spotify &spotify;

	QTreeWidget *defaultTree(const QStringList &headers);
	void albumClick(QTreeWidgetItem *item, int column);
	void albumMenu(const QPoint &pos);
	void artistClick(QListWidgetItem *item);
	void playlistClick(QListWidgetItem *item);
	void playlistMenu(const QPoint &pos);
	void search();
	void trackClick(QTreeWidgetItem *item, int column);
	void trackMenu(const QPoint &pos);

protected:
	void showEvent(QShowEvent *event) override;
	void hideEvent(QHideEvent *event) override;
};