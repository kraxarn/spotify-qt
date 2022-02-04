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
#include "view/context/abstractalbumcontent.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QResizeEvent>

namespace Context
{
	class AbstractAlbumContent: public QWidget
	{

	Q_OBJECT

	public:
		AbstractAlbumContent(QWidget *parent) 
			: QWidget(parent){};

		~AbstractAlbumContent(){ }
        
		virtual void reset();

		// virtual auto getCurrentlyPlaying() const -> const lib::spt::track &;
		virtual void setCurrentlyPlaying(const lib::spt::track &track) = 0;

		virtual void setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage) = 0;

	// private:
	// 	/** Width and height of album */
	// 	// static constexpr int albumSize = 300;

	// 	lib::spt::api &spotify;
	// 	spt::Current &current;
	// 	const lib::cache &cache;

	// 	lib::spt::track currentlyPlaying;

	// 	AlbumCover *album = nullptr;
	// 	NowPlaying *nowPlaying = nullptr;

	// 	void onSongMenu(const QPoint &pos);
	};
}
