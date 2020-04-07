#pragma once

#include <QString>

namespace spt
{
	class Item
	{
	public:
		virtual QString itemName()	= 0;
		virtual QString itemId()	= 0;
	};
}