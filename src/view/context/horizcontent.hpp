#pragma once

#include "lib/developermode.hpp"
#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "spotify/current.hpp"
#include "menu/track.hpp"
#include "util/icon.hpp"
#include "util/image.hpp"
#include "view/context/nowplaying.hpp"
#include "view/context/albumcover.hpp"
#include "view/context/contentinterface.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QResizeEvent>

namespace Context
{
	class HorizContent: public ContentInterface
	{

	public:
		HorizContent(lib::spt::api &spotify, spt::Current &current,
			const lib::cache &cache, QWidget *parent);

		void reset();

		void setCurrentlyPlaying(const lib::spt::track &track);

		void setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage);

	protected:
		void resizeEvent(QResizeEvent *event);

	private:
		lib::spt::api &spotify;
		spt::Current &current;
		const lib::cache &cache;

		lib::spt::track currentlyPlaying;

		AlbumCover *album = nullptr;
		NowPlaying *nowPlaying = nullptr;

		void onSongMenu(const QPoint &pos);
	};
}
