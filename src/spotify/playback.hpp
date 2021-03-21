#pragma once

#include "lib/spotify/track.hpp"
#include "lib/spotify/device.hpp"
#include "../util/jsonutils.hpp"

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
		QJsonObject toJson() const;

		int volume() const;

		QString contextType;
		QString contextUri;
		QString repeat = "off";
		lib::spt::track item = lib::spt::track();
		bool isPlaying = false;
		bool shuffle = false;
		int progressMs = 0;
		lib::spt::device device;
	};
}