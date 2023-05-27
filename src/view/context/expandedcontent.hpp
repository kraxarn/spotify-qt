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
		ExpandedContent(lib::spt::api &spotify, const lib::cache &cache,
			lib::settings &settings, QWidget *parent);

		void setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage) override;

	protected:
		auto iconSize() const -> QSize override;

	private:
		QPixmap albumPixmap;

		void scaleAlbum(int width);

		void resizeEvent(QResizeEvent *event) override;
	};
}
