#pragma once

#include "lib/enums.hpp"

#include <QComboBox>

class AlbumShapeComboBox: public QComboBox
{
public:
	explicit AlbumShapeComboBox(QWidget *parent);

	void addAlbumShape(lib::album_shape albumShape);
	auto currentAlbumShape() -> lib::album_shape;

private:
	static auto getAlbumShape(const QVariant &data) -> lib::album_shape;
};
