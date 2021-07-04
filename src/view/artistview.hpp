#pragma once

#include "lib/enum/followtype.hpp"
#include "menu/albummenu.hpp"
#include "menu/songmenu.hpp"
#include "spotify/spotify.hpp"
#include "widget/loader.hpp"
#include "view/artistcover.hpp"

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
	ArtistView(spt::Spotify &spotify, const std::string &artistId,
		lib::cache &cache, const lib::http_client &httpClient, QWidget *parent);

	std::function<void(const lib::spt::artist &artist)> onArtistLoaded;

private:
	void artistLoaded(const lib::spt::artist &loadedArtist);
	void topTracksLoaded(const std::vector<lib::spt::track> &tracks);
	void albumsLoaded(const std::vector<lib::spt::album> &albums);
	void relatedArtistsLoaded(const std::vector<lib::spt::artist> &artists);

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
	View::ArtistCover *coverLabel = nullptr;
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
	std::string artistId;
	lib::spt::artist artist;
	spt::Spotify &spotify;
	lib::cache &cache;
	const lib::http_client &httpClient;
};
