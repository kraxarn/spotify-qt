#pragma once

#include "lib/enum/albumshape.hpp"

#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QVariant>

class Image
{
public:
	/**
	 * Mask image using specified shape
	 */
	static auto mask(const QPixmap &source, lib::album_shape shape = lib::album_shape::app,
		const QVariant &data = QVariant()) -> QPixmap;

private:
	Image() = default;

	static void addAppShape(QPainterPath &path, const QImage &img);
	static void addPieShape(QPainterPath &path, const QImage &img, const QVariant &data);
};
