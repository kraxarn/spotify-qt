#pragma once

#include "lib/enum/followtype.hpp"
#include "lib/spotify/api.hpp"

#include "menu/album.hpp"
#include "menu/songmenu.hpp"

#include "view/artist/albumslist.hpp"
#include "view/artist/cover.hpp"
#include "view/artist/playbutton.hpp"
#include "view/artist/searchmenu.hpp"
#include "view/artist/sharemenu.hpp"
#include "view/artist/title.hpp"
#include "view/artist/trackslist.hpp"

#include "util/parent.hpp"

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

		private:
			void artistLoaded(const lib::spt::artist &loadedArtist);
			void topTracksLoaded(const std::vector<lib::spt::track> &tracks);
			void relatedArtistsLoaded(const std::vector<lib::spt::artist> &artists);

			void relatedClick(QListWidgetItem *item);

			std::string artistId;
			lib::spt::artist artist;
			lib::spt::api &spotify;
			lib::cache &cache;
			const lib::http_client &httpClient;

			View::Artist::AlbumsList *albumList;
			View::Artist::Cover *coverLabel = nullptr;
			View::Artist::PlayButton *context = nullptr;
			View::Artist::Title *name = nullptr;
			View::Artist::TracksList *topTracksList = nullptr;

			QVBoxLayout *layout = nullptr;
			QHBoxLayout *title = nullptr;
			QLabel *genres = nullptr;
			QListWidget *relatedList = nullptr;
			QTabWidget *tabs = nullptr;
		};
	}
}
