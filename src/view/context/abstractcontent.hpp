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
#include <QResizeEvent>

namespace Context
{
	class AbstractContent: public QWidget
	{
	public:
		AbstractContent(lib::spt::api &spotify, const lib::cache &cache, QWidget *parent);

		void reset();
		auto getCurrentlyPlaying() const -> const lib::spt::track &;
		void setCurrentlyPlaying(const lib::spt::track &track);
		void setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage);

	protected:
		lib::spt::api &spotify;
		const lib::cache &cache;

		QLabel *album = nullptr;
		NowPlaying *nowPlaying = nullptr;

		lib::spt::track currentlyPlaying;

		template<typename T>
		auto layout() -> T *
		{
			if (QWidget::layout() == nullptr)
			{
				new T(this);
				QWidget::layout()->setSpacing(0);
			}

			return qobject_cast<T *>(QWidget::layout());
		}

	private:
		void onSongMenu(const QPoint &pos);
	};
}
