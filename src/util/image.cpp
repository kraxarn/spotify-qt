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

	if (shape == lib::album_shape::app)
	{
		addAppShape(path, img);
	}
	else if (shape == lib::album_shape::circle)
	{
		addCircleShape(path, img.size());
	}
	else if (shape == lib::album_shape::none && data.canConvert<int>())
	{
		addPieShape(path, img, data);
	}

	painter.setClipPath(path);
	painter.drawImage(0, 0, img);
	return QPixmap::fromImage(out);
}

void Image::addAppShape(QPainterPath &path, const QImage &img)
{
	auto width = (float) img.width();
	auto height = (float) img.height();

	constexpr float quarter = 4.F;

	auto quarterWidth = width / quarter;
	auto quarterHeight = height / quarter;

	path.addPolygon(QPolygonF()
		<< QPointF(quarterWidth, 0)
		<< QPointF(width, 0)
		<< QPointF(width, quarterHeight * 3)
		<< QPointF(quarterWidth * 3, height)
		<< QPointF(0, height)
		<< QPointF(0, quarterHeight));
}

void Image::addCircleShape(QPainterPath &path, const QSize &size)
{
	path.addEllipse(QRectF({}, size));
}

void Image::addPieShape(QPainterPath &path, const QImage &img, const QVariant &data)
{
	auto width = (float) img.width();
	auto height = (float) img.height();

	constexpr float half = 2.F;
	constexpr int quarter = 25;

	auto halfWidth = width / half;
	auto halfHeight = height / half;

	QPolygonF polygon;
	switch (data.toInt() / quarter)
	{
		case 0:
			polygon = QPolygonF(QRectF(halfWidth, 0,
				halfWidth, halfHeight));
			break;

		case 1:
			polygon = QPolygonF(QRectF(halfWidth, 0,
				halfWidth, height));
			break;

		case 2:
			polygon
				<< QPointF(halfWidth, 0)
				<< QPointF(halfWidth, halfHeight)
				<< QPointF(0, halfHeight)
				<< QPointF(0, height)
				<< QPointF(width, height)
				<< QPointF(width, 0);
			break;

		case 3:
			polygon = QPolygonF(QRectF(0, 0, width, height));
	}

	path.addPolygon(polygon);
}
