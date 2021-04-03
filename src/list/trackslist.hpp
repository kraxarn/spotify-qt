#pragma once

#include "../spotify/spotify.hpp"
#include "lib/cache.hpp"
#include "../spotify/current.hpp"
#include "../menu/songmenu.hpp"

#include <QListWidget>
#include <QTreeWidgetItem>

class TracksList: public QTreeWidget
{
Q_OBJECT

public:
	TracksList(spt::Spotify &spotify, lib::settings &settings, lib::cache &cache,
		QWidget *parent);

	void updateResizeMode(lib::resize_mode mode);
	void setPlayingTrackItem(QTreeWidgetItem *item);
	void setPlayingTrackItem(const std::string &itemId);

	/**
	 * Load tracks directly, without cache, but select an item
	 */
	void load(const std::vector<lib::spt::track> &tracks, const std::string &selectedId);

	/**
	 * Load tracks directly, without cache
	 */
	void load(const std::vector<lib::spt::track> &tracks);

	/**
	 * Load playlist first from cache, then refresh it
	 */
	void load(const lib::spt::playlist &playlist);

	/**
	 * Load album first from cache, then refresh it
	 */
	void load(const lib::spt::album &album);

protected:
	void resizeEvent(QResizeEvent *event) override;

private:
	void menu(const QPoint &pos);
	void clicked(QTreeWidgetItem *item, int column);
	void headerMenu(const QPoint &pos);
	void resizeHeaders(const QSize &newSize);
	const spt::Current &getCurrent();

	// lib
	lib::settings &settings;
	lib::cache &cache;
	// spt
	spt::Spotify &spotify;
	// std
	std::unordered_map<std::string, QTreeWidgetItem *> trackItems;
	// qt
	QTreeWidgetItem *playingTrackItem = nullptr;
	QIcon emptyIcon;
};