#pragma once

#include <QString>

namespace spt
{
	struct Device
	{
		QString id, name, type;
		bool isActive, isPrivateSession, isRestricted;
		uint8_t volumePercent;
	};
}