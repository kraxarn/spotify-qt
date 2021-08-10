#pragma once

#include "lib/spotify/track.hpp"
#include "menu/songmenu.hpp"

#include <QLabel>

namespace View
{
	namespace Context
	{
		class NowPlaying: public QWidget
		{
		Q_OBJECT

		public:
			NowPlaying(QWidget *parent);

			void setTrack(const lib::spt::track &track);
			void setNoPlaying();

		private:
			static constexpr float nameScale = 1.1F;
			static constexpr float artistScale = 0.9F;

			QVBoxLayout *layout = nullptr;

			QLabel *artist = nullptr;
			QLabel *name = nullptr;

			auto newLabel(float scale) -> QLabel *;
		};
	}
}
