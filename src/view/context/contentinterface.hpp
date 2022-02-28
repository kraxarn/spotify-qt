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
	class ContentInterface: public QWidget
	{

	Q_OBJECT

	public:
		ContentInterface(QWidget *parent) : QWidget(parent) {}

		virtual void reset() = 0;

		virtual void setCurrentlyPlaying(const lib::spt::track &track) = 0;

		virtual void setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage) = 0;

		virtual ~ContentInterface() {};
	};
}
