#pragma once

#include "enum/datarole.hpp"

#include <string>

namespace ListItem
{
	class Library
	{
	public:
		Library(const std::string &name, const std::string &id, DataRole role);

		std::string name;
		std::string id;
		DataRole role;
	};
}
