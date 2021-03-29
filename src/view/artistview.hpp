#pragma once

#include "lib/enum/followtype.hpp"
#include "../mainwindow.hpp"
#include "../menu/albummenu.hpp"
#include "../menu/songmenu.hpp"
#include "../spotify/spotify.hpp"
#include "../loader.hpp"

#include <QDockWidget>
#include <QLabel>
#include <QListWidget>
#include <QTabWidget>
#include <QToolTip>
#include <QTreeWidget>
#include <QVBoxLayout>

class ArtistView: public QWidget
{
Q_OBJECT

public:
	ArtistView(spt::Spotify &spotify, const std::string &artistId, const lib::settings &settings,
		QWidget	*parent);

	std::function<void(const spt::Artist &artist)> onArtistLoaded;

private:
	void artistLoaded(const spt::Artist &loadedArtist);
	void topTracksLoaded(const std::vector<lib::spt::track> &tracks);
	void albumsLoaded(const std::vector<lib::spt::album> &albums);
	void relatedArtistsLoaded(const std::vector<spt::Artist> &artists);

	void updateFollow(bool isFollowing);
	void follow(bool checked);
	void trackClick(QListWidgetItem *item);
	void trackMenu(const QPoint &pos);
	void loadAlbumId(QTreeWidgetItem *item);
	void relatedClick(QListWidgetItem *item);
	void albumMenu(const QPoint &pos);
	void albumDoubleClicked(QTreeWidgetItem *item, int column);
	void play(bool checked);
	void searchWikipedia(bool checked);
	void searchDuckDuckGo(bool checked);
	void copyLink(bool checked);
	void openInSpotify(bool checked);

	QVBoxLayout *layout = nullptr;
	QHBoxLayout *title = nullptr;
	QLabel *coverLabel = nullptr;
	QLabel *name = nullptr;
	QMenu *menu = nullptr;
	QToolButton *context = nullptr;
	QLabel *genres = nullptr;

	QAction *popularity = nullptr;
	QListWidget *relatedList = nullptr;
	QListWidget *topTracksList = nullptr;
	QAction *followButton = nullptr;
	std::vector<std::string> topTrackIds;
	QTabWidget *tabs = nullptr;
	QTreeWidget *albumList = nullptr;
	QTreeWidget *singleList = nullptr;
	QString artistId;
	spt::Artist artist;
	spt::Spotify &spotify;
};