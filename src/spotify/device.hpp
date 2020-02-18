#pragma once

#include <QString>

namespace spt
{
	struct Device
	{
		QString id, name, type;
		bool isActive = false, isPrivateSession = false, isRestricted = false;
		unsigned int volumePercent = 0u;
	};
}