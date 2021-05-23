#include "imageutils.hpp"

auto ImageUtils::mask(const QPixmap &source, MaskShape shape,
	const QVariant &data) -> QPixmap
{
	if (source.isNull())
	{
		return source;
	}

	auto img = source.toImage().convertToFormat(QImage::Format_ARGB32);
	QImage out(img.size(), QImage::Format_ARGB32);
	out.fill(Qt::GlobalColor::transparent);
	QPainter painter(&out);
	painter.setOpacity(0);
	painter.setBrush(Qt::white);
	painter.setPen(Qt::NoPen);
	painter.drawImage(0, 0, img);
	painter.setOpacity(1);
	QPainterPath path(QPointF(0, 0));

	QPolygonF polygon;
	switch (shape)
	{
		case MaskShape::App:
			polygon << QPointF(img.width() / 4.f, 0)
				<< QPointF(img.width(), 0)
				<< QPointF(img.width(), (img.height() / 4.f) * 3)
				<< QPointF((img.width() / 4.f) * 3, img.height())
				<< QPointF(0, img.height())
				<< QPointF(0, img.height() / 4.f);
			break;

		case MaskShape::Pie:
			switch (data.toInt() / 25)
			{
				case 0:
					polygon = QPolygonF(QRectF(img.width() / 2.f, 0,
						img.width() / 2.f, img.height() / 2.f));
					break;

				case 1:
					polygon = QPolygonF(QRectF(img.width() / 2.f, 0,
						img.width() / 2.f, img.height()));
					break;

				case 2:
					polygon << QPointF(img.width() / 2.f, 0)
						<< QPointF(img.width() / 2.f, img.height() / 2.f)
						<< QPointF(0, img.height() / 2.f)
						<< QPointF(0, img.height())
						<< QPointF(img.width(), img.height())
						<< QPointF(img.width(), 0);
					break;

				case 3:
					polygon = QPolygonF(QRectF(0, 0, img.width(), img.height()));
					break;
			}
			break;
	}

	path.addPolygon(polygon);
	painter.setClipPath(path);
	painter.drawImage(0, 0, img);
	return QPixmap::fromImage(out);
}
