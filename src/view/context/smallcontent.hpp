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

namespace Context
{
	class SmallContent: public AbstractContent
	{
	Q_OBJECT

	public:
		SmallContent(lib::spt::api &spotify, const lib::cache &cache,
			lib::settings &settings, QWidget *parent);

	protected:
		auto iconSize() const -> QSize override;

	private:
		/** Width and height of album */
		static constexpr int albumSize = 64;
	};
}
