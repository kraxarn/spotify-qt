#include "widget/albumshapecombobox.hpp"

#include "lib/strings.hpp"
#include "util/image.hpp"

#include <QApplication>

AlbumShapeComboBox::AlbumShapeComboBox(QWidget *parent)
	: QComboBox(parent)
{
}

void AlbumShapeComboBox::addAlbumShape(lib::album_shape albumShape)
{
	const auto shapeName = lib::enums<lib::album_shape>::to_string(albumShape);
	const auto text = QString::fromStdString(lib::strings::capitalize(shapeName));
	const auto data = static_cast<short>(albumShape);

	auto textColor = QApplication::palette().text().color();
	textColor.setAlpha(192);

	QPixmap icon(64, 64);
	icon.fill(textColor);

	addItem({Image::mask(icon, albumShape)}, text, data);
}

auto AlbumShapeComboBox::getAlbumShape(const QVariant &data) -> lib::album_shape
{
	if (!data.canConvert<short>())
	{
		return lib::album_shape::none;
	}

	const auto value = data.value<short>();
	return static_cast<lib::album_shape>(value);
}

auto AlbumShapeComboBox::currentAlbumShape() -> lib::album_shape
{
	return getAlbumShape(currentData());
}
