#pragma once

#include <QLabel>
#include <QResizeEvent>

namespace View
{
	class ArtistCover: public QLabel
	{
	public:
		explicit ArtistCover(QWidget *parent);

		void setJpeg(const QByteArray &jpeg);

	protected:
		void resizeEvent(QResizeEvent *event) override;

	private:
		static constexpr int maxHeight = 160;

		QPixmap cover;

		void scaleCover(int width);
	};
}
