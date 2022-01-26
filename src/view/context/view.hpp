#pragma once

#include "view/context/title.hpp"
#include "view/context/horizcontent.hpp"

#include <QDockWidget>

namespace Context
{
	class View: public QDockWidget
	{
	Q_OBJECT

	public:
		View(lib::spt::api &spotify, spt::Current &current,
			const lib::cache &cache, QWidget *parent);

		void updateContextIcon();
		void resetCurrentlyPlaying() const;

		auto getCurrentlyPlaying() const -> const lib::spt::track &;
		void setCurrentlyPlaying(const lib::spt::track &track) const;

		void setAlbum(const lib::spt::entity &albumEntity, const QPixmap &albumImage) const;

	private:
		Title *title = nullptr;
		HorizContent *content = nullptr;
	};
}
