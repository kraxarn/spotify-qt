#pragma once

#include <QString>

namespace spt
{
	typedef struct StructDevice
	{
		QString id, name, type;
		bool isActive = false;
	} Device;
}