#pragma once

namespace spt { class Track; }

#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace spt
{
	class Track
	{
	public:
		QDateTime addedAt;
		bool isLocal;

		Track();
		explicit Track(QJsonObject item);

		QString id();
		QString album();
		QString albumId();
		QString artist();
		QString artistId();
		QString name();
		int duration();
		QString image();

	private:
		QJsonObject track;
	};
}