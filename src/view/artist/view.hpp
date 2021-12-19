#pragma once

#include "lib/enum/followtype.hpp"
#include "lib/spotify/api.hpp"

#include "menu/album.hpp"
#include "menu/songmenu.hpp"

#include "view/artist/albumslist.hpp"
#include "view/artist/cover.hpp"
#include "view/artist/playbutton.hpp"
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

namespace Artist
{
	class View: public QWidget
	{
	Q_OBJECT

	public:
		View(lib::spt::api &spotify, const std::string &artistId,
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

		AlbumsList *albumList;
		Cover *coverLabel = nullptr;
		PlayButton *context = nullptr;
		Title *name = nullptr;
		TracksList *topTracksList = nullptr;

		QVBoxLayout *layout = nullptr;
		QHBoxLayout *title = nullptr;
		QLabel *genres = nullptr;
		QListWidget *relatedList = nullptr;
		QTabWidget *tabs = nullptr;
	};
}
