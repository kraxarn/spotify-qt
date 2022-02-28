#pragma once

#include "lib/developermode.hpp"
#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "spotify/current.hpp"
#include "menu/track.hpp"
#include "util/icon.hpp"
#include "util/image.hpp"
#include "view/context/nowplaying.hpp"
#include "view/context/abstractcontent.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>

namespace Context
{
	class Content: public AbstractContent
	{

	public:
		Content(lib::spt::api &spotify, spt::Current &current,
			const lib::cache &cache, QWidget *parent);

		void reset();

		// lib::spt::track& getCurrentlyPlaying() const;
		void setCurrentlyPlaying(const lib::spt::track &track);

		void setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage);

	private:
		/** Width and height of album */
		static constexpr int albumSize = 64;

		lib::spt::api &spotify;
		spt::Current &current;
		const lib::cache &cache;

		QLabel *album = nullptr;
		NowPlaying *nowPlaying = nullptr;

		void onSongMenu(const QPoint &pos);
	};
}
