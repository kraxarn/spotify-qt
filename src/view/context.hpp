#pragma once

#include "view/contexttitle.hpp"
#include "view/contextcontent.hpp"

#include <QDockWidget>

namespace View
{
	class Context: public QDockWidget
	{
	public:
		Context(lib::spt::api &spotify, const lib::settings &settings,
			spt::Current &current, const lib::cache &cache, QWidget *parent);

		void updateContextIcon();
		void resetCurrentlyPlaying();

		auto getCurrentlyPlaying() const -> const lib::spt::track &;
		void setCurrentlyPlaying(const lib::spt::track &track);

		void setAlbum(const QPixmap &pixmap);

	private:
		View::ContextTitle *title = nullptr;
		View::ContextContent *content = nullptr;
	};
}
