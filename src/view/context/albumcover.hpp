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

	// protected:
		// void resizeEvent(QResizeEvent *event) override;
		void scaleCover(int width, int height);

	private:
		static constexpr int maxHeight = 160;

		QPixmap cover;

		// void scaleCover(int width);
	};
}
