#pragma once

#include "lib/spotify/artist.hpp"
#include "lib/spotify/api.hpp"

#include "util/icon.hpp"
#include "view/artist/searchmenu.hpp"
#include "view/artist/sharemenu.hpp"

#include <QToolButton>
#include <QMenu>

namespace View
{
	namespace Artist
	{
		class PlayButton: public QToolButton
		{
		public:
			PlayButton(lib::spt::api &spotify, QWidget *parent);

			void updateFollow(bool isFollowing);
			void setArtist(const lib::spt::artist &artist);

		private:
			lib::spt::api &spotify;
			lib::spt::artist artist;

			QAction *popularity = nullptr;
			QAction *follow = nullptr;

			auto contextMenu() -> QMenu *;

			void onClicked(bool checked);
			void onFollow(bool checked);
		};
	}
}
