#include "util/image.hpp"

auto Image::mask(const QPixmap &source, lib::album_shape shape,
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
	if (shape == lib::album_shape::app)
	{
		polygon = appShape(img);
	}
	else if (shape == lib::album_shape::none && data.canConvert<int>())
	{
		polygon = pieShape(img, data);
	}

	path.addPolygon(polygon);
	painter.setClipPath(path);
	painter.drawImage(0, 0, img);
	return QPixmap::fromImage(out);
}

auto Image::appShape(const QImage &img) -> QPolygonF
{
	auto width = (float) img.width();
	auto height = (float) img.height();

	constexpr float quarter = 4.F;

	auto quarterWidth = width / quarter;
	auto quarterHeight = height / quarter;

	return QPolygonF()
		<< QPointF(quarterWidth, 0)
		<< QPointF(width, 0)
		<< QPointF(width, quarterHeight * 3)
		<< QPointF(quarterWidth * 3, height)
		<< QPointF(0, height)
		<< QPointF(0, quarterHeight);
}

auto Image::pieShape(const QImage &img, const QVariant &data) -> QPolygonF
{
	auto width = (float) img.width();
	auto height = (float) img.height();

	constexpr float half = 2.F;
	constexpr int quarter = 25;

	auto halfWidth = width / half;
	auto halfHeight = height / half;

	switch (data.toInt() / quarter)
	{
		case 0:
			return QPolygonF(QRectF(halfWidth, 0,
				halfWidth, halfHeight));

		case 1:
			return QPolygonF(QRectF(halfWidth, 0,
				halfWidth, height));

		case 2:
			return QPolygonF()
				<< QPointF(halfWidth, 0)
				<< QPointF(halfWidth, halfHeight)
				<< QPointF(0, halfHeight)
				<< QPointF(0, height)
				<< QPointF(width, height)
				<< QPointF(width, 0);

		case 3:
			return QPolygonF(QRectF(0, 0, width, height));

		default:
			return QPolygonF();
	}
}
