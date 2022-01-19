#pragma once

#include "lib/developermode.hpp"
#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "spotify/current.hpp"
#include "menu/track.hpp"
#include "util/icon.hpp"
#include "util/image.hpp"
#include "view/context/nowplaying.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>

namespace Context
{
	class Content: public QWidget
	{
	Q_OBJECT

	public:
		Content(lib::spt::api &spotify, spt::Current &current,
			const lib::cache &cache, QWidget *parent);

		void reset();

		auto getCurrentlyPlaying() const -> const lib::spt::track &;
		void setCurrentlyPlaying(const lib::spt::track &track);

		void setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage);
		void setAlbumJpg(const lib::spt::entity &albumEntity, const std::string &data);

	private:
		/** Width and height of album */
		static constexpr int albumSize = 300;

		lib::spt::api &spotify;
		spt::Current &current;
		const lib::cache &cache;

		lib::spt::track currentlyPlaying;

		QLabel *album = nullptr;
		NowPlaying *nowPlaying = nullptr;

		void onSongMenu(const QPoint &pos);
	};
}
