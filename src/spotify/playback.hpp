#pragma once

#include "track.hpp"

#include <QJsonObject>

namespace spt
{
	class Playback
	{
	public:
		Playback();
		Playback(const QJsonObject &json);

		int		progressMs	= 0;
		Track	item		= Track();
		bool	isPlaying	= false;
		int		volume		= 100;
		QString	repeat		= "off";
		bool	shuffle		= false;
	};
}