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

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QResizeEvent>

namespace Context
{
	class AbstractContent: public QWidget
	{

	Q_OBJECT

	public:
		AbstractContent(lib::spt::api &spotify, spt::Current &current,
	const lib::cache &cache, QWidget *parent);

		void reset();
		
		auto getCurrentlyPlaying() const -> const lib::spt::track &;

		void setCurrentlyPlaying(const lib::spt::track &track);

		void setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage);

		virtual ~AbstractContent() {};

	protected:
		lib::spt::api &spotify;
		spt::Current &current;
		const lib::cache &cache;

		AlbumCover *album = nullptr;
		NowPlaying *nowPlaying = nullptr;
		
		lib::spt::track currentlyPlaying;
		void onSongMenu(const QPoint &pos);
	
	};
}
