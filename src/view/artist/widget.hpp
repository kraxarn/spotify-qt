#pragma once

#include "lib/enum/followtype.hpp"
#include "lib/spotify/api.hpp"

#include "menu/albummenu.hpp"
#include "menu/songmenu.hpp"
#include "widget/loader.hpp"
#include "view/artist/cover.hpp"
#include "view/artist/albumlist.hpp"

#include <QDockWidget>
#include <QLabel>
#include <QListWidget>
#include <QTabWidget>
#include <QToolTip>
#include <QTreeWidget>
#include <QVBoxLayout>

namespace View
{
	namespace Artist
	{
		class Widget: public QWidget
		{
		Q_OBJECT

		public:
			Widget(lib::spt::api &spotify, const std::string &artistId,
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
			void relatedClick(QListWidgetItem *item);
			void play(bool checked);
			void searchWikipedia(bool checked);
			void searchDuckDuckGo(bool checked);
			void copyLink(bool checked);
			void openInSpotify(bool checked);

			QVBoxLayout *layout = nullptr;
			QHBoxLayout *title = nullptr;
			View::Artist::Cover *coverLabel = nullptr;
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
			View::Artist::AlbumList *albumList = nullptr;
			View::Artist::AlbumList *singleList = nullptr;
			std::string artistId;
			lib::spt::artist artist;
			lib::spt::api &spotify;
			lib::cache &cache;
			const lib::http_client &httpClient;
		};
	}
}
