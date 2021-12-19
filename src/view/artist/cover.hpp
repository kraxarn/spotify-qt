#pragma once

#include <QLabel>
#include <QResizeEvent>

namespace Artist
{
	class Cover: public QLabel
	{
	public:
		explicit Cover(QWidget *parent);

		void setJpeg(const QByteArray &jpeg);

	protected:
		void resizeEvent(QResizeEvent *event) override;

	private:
		static constexpr int maxHeight = 160;

		QPixmap cover;

		void scaleCover(int width);
	};
}
