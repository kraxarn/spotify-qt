#pragma once

#include "enum/datarole.hpp"

#include <string>

class LibraryItem
{
public:
	LibraryItem(const std::string &name, const std::string &id, DataRole role);

	std::string name;
	std::string id;
	DataRole role;
};