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
		Playback();
		Playback(const QJsonObject &json);
		QMap<QString, QVariant> metadata();

		int		progressMs	= 0;
		Track	item		= Track();
		bool	isPlaying	= false;
		int		volume		= 100;
		QString	repeat		= "off";
		bool	shuffle		= false;
	};
}