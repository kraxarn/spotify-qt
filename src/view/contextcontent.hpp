#pragma once

#include "lib/developermode.hpp"
#include "lib/spotify/api.hpp"
#include "lib/cache.hpp"

#include "spotify/current.hpp"
#include "menu/songmenu.hpp"
#include "util/icon.hpp"
#include "util/imageutils.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>

namespace View
{
	class ContextContent: public QWidget
	{
	public:
		ContextContent(lib::spt::api &spotify, spt::Current &current,
			const lib::cache &cache, QWidget *parent);

		void reset();

		auto getCurrentlyPlaying() const -> const lib::spt::track &;
		void setCurrentlyPlaying(const lib::spt::track &track);

		void setAlbum(const QPixmap &pixmap);

	private:
		lib::spt::api &spotify;
		spt::Current &current;
		const lib::cache &cache;

		lib::spt::track currentlyPlaying;

		QLabel *album = nullptr;
		QLabel *nowPlaying = nullptr;

		void onSongMenu(const QPoint &pos);
	};
}
