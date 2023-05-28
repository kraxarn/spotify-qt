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
		AbstractContent(lib::spt::api &spotify, const lib::cache &cache,
			lib::settings &settings, QWidget *parent);

		void reset();
		void setCurrentlyPlaying(const lib::spt::track &track);
		virtual void setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage);
		auto isCurrentlyPlaying() const -> bool;

	protected:
		lib::spt::api &spotify;
		const lib::cache &cache;
		lib::settings &settings;

		QLabel *album = nullptr;
		NowPlaying *nowPlaying = nullptr;
		bool isPlaying;

		virtual auto iconSize() const -> QSize = 0;

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
