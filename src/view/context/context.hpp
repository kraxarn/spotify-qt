#pragma once

#include "view/context/title.hpp"
#include "view/context/content.hpp"

#include <QDockWidget>

namespace View
{
	namespace Context
	{
		class Context: public QDockWidget
		{
		Q_OBJECT

		public:
			Context(lib::spt::api &spotify, spt::Current &current,
				const lib::cache &cache, QWidget *parent);

			void updateContextIcon();
			void resetCurrentlyPlaying() const;

			auto getCurrentlyPlaying() const -> const lib::spt::track &;
			void setCurrentlyPlaying(const lib::spt::track &track) const;

			void setAlbum(const QPixmap &pixmap) const;

		private:
			View::Context::Title *title = nullptr;
			View::Context::Content *content = nullptr;
		};
	}
}
