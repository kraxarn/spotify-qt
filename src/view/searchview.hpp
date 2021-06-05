#pragma once

#include "mainwindow.hpp"
#include "spotify/spotify.hpp"
#include "lib/httpclient.hpp"

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
	explicit SearchView(spt::Spotify &spotify, const lib::settings &settings, lib::cache &cache,
		const lib::http_client &httpClient, QWidget *parent);

private:
	QTabWidget *tabs = nullptr;
	QLineEdit *searchBox = nullptr;
	QListWidget *artistList = nullptr;
	QListWidget *playlistList = nullptr;
	QTreeWidget *albumList = nullptr;
	QTreeWidget *trackList = nullptr;
	spt::Spotify &spotify;
	lib::cache &cache;
	const lib::http_client &httpClient;

	auto defaultTree(const QStringList &headers) -> QTreeWidget *;
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

private:
	void addArtist(const lib::spt::artist &artist);
	void addAlbum(const lib::spt::album &album);
	void addPlaylist(const lib::spt::playlist &playlist);
	void addTrack(const lib::spt::track &track);

	void resultsLoaded(const lib::spt::search_results &results);
};
