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
#include "view/context/abstractcontent.hpp"

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QResizeEvent>

namespace Context
{
	class HorizContent: public AbstractContent
	{

	public:
		HorizContent(lib::spt::api &spotify, spt::Current &current,
			const lib::cache &cache, QWidget *parent);

		void resizeEvent(QResizeEvent *event); 
	};
}
