#pragma once

#include "lib/log.hpp"

#include <QLabel>
#include <QResizeEvent>

namespace Context
{
	class AlbumCover: public QLabel
	{
	public:
		explicit AlbumCover(QWidget *parent);

		void setPixmap(const QPixmap &pixmap);

		void scaleCover(int width, int height);

	private:
		QPixmap cover;
	};
}
