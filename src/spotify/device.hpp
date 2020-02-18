#pragma once

#include <QString>

namespace spt
{
	typedef struct StructDevice
	{
		QString id, name, type;
		bool isActive = false, isPrivateSession = false, isRestricted = false;
		unsigned int volumePercent = 0u;
	} Device;
}