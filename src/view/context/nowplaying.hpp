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
			static constexpr float nameScale = 1.05F;
			static constexpr float artistScale = 0.95F;

			QVBoxLayout *layout = nullptr;

			QLabel *artist = nullptr;
			QLabel *name = nullptr;

			auto newLabel(float scale) -> QLabel *;
		};
	}
}
