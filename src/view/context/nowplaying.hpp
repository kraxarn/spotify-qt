#pragma once

#include "lib/spotify/track.hpp"
#include "menu/songmenu.hpp"

#include <QLabel>

namespace View
{
	namespace Context
	{
		class NowPlaying: public QLabel
		{
		public:
			NowPlaying(QWidget *parent);

			void setTrack(const lib::spt::track &track);
			void setNoPlaying();
		};
	}
}
