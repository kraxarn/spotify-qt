#pragma once

#include "track.hpp"

#include <QJsonObject>
#include <QMap>
#include <QString>
#include <QVariant>

namespace spt
{
	class Playback
	{
	public:
		Playback() = default;
		explicit Playback(const QJsonObject &json);
		QVariantMap metadata() const;

		QString contextType;
		QString contextUri;
		QString repeat = "off";
		Track item = Track();
		bool isPlaying = false;
		bool shuffle = false;
		int progressMs = 0;
		int volume = 100;
	};
}