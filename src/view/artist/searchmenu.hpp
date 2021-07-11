#pragma once

#include "lib/spotify/artist.hpp"
#include "util/icon.hpp"
#include "util/urlutils.hpp"

#include <QMenu>

namespace View
{
	namespace Artist
	{
		class SearchMenu: public QMenu
		{
		public:
			SearchMenu(const lib::spt::artist &artist, QWidget *parent);

		private:
			const lib::spt::artist &artist;

			void onWikipedia(bool checked);
			void onDuckDuckGo(bool checked);
		};
	}
}
