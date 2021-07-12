#pragma once

#include "lib/enum/followtype.hpp"
#include "lib/spotify/api.hpp"

#include "menu/albummenu.hpp"
#include "menu/songmenu.hpp"
#include "widget/loader.hpp"

#include "view/artist/cover.hpp"
#include "view/artist/albumlist.hpp"
#include "view/artist/searchmenu.hpp"
#include "view/artist/sharemenu.hpp"
#include "view/artist/playbutton.hpp"

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
		class Artist: public QWidget
		{
		Q_OBJECT

		public:
			Artist(lib::spt::api &spotify, const std::string &artistId,
				lib::cache &cache, const lib::http_client &httpClient, QWidget *parent);

			std::function<void(const lib::spt::artist &artist)> onArtistLoaded;

		private:
			void artistLoaded(const lib::spt::artist &loadedArtist);
			void topTracksLoaded(const std::vector<lib::spt::track> &tracks);
			void relatedArtistsLoaded(const std::vector<lib::spt::artist> &artists);

			void trackClick(QListWidgetItem *item);
			void trackMenu(const QPoint &pos);
			void relatedClick(QListWidgetItem *item);

			QVBoxLayout *layout = nullptr;
			QHBoxLayout *title = nullptr;
			View::Artist::Cover *coverLabel = nullptr;
			QLabel *name = nullptr;
			View::Artist::PlayButton *context = nullptr;
			QLabel *genres = nullptr;

			QListWidget *relatedList = nullptr;
			QListWidget *topTracksList = nullptr;
			std::vector<std::string> topTrackIds;
			QTabWidget *tabs = nullptr;
			View::Artist::AlbumList *albumList;
			std::string artistId;
			lib::spt::artist artist;
			lib::spt::api &spotify;
			lib::cache &cache;
			const lib::http_client &httpClient;
		};
	}
}
