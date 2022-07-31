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
#include <QResizeEvent>

namespace Context
{
	class ExpandedContent: public AbstractContent
	{
	Q_OBJECT

	public:
		ExpandedContent(lib::spt::api &spotify, spt::Current &current,
			const lib::cache &cache, QWidget *parent);

	private:
		void resizeEvent(QResizeEvent *event) override;
	};
}