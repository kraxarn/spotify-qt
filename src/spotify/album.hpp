#pragma once

#include "item.hpp"

#include <QJsonObject>
#include <QString>
#include <QJsonArray>
#include <QDateTime>

namespace spt
{
	class Album : public Item
	{
	public:
		explicit Album(const QJsonObject &json);
		QString id, albumGroup, image, name, artist;
		QDateTime releaseDate;

		QString itemName() override;
		QString itemId() override;
	};
}