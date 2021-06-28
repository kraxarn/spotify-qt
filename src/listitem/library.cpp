#include "listitem/library.hpp"

ListItem::Library::Library(const std::string &name, const std::string &id, DataRole role)
	: name(std::string(name)),
	id(std::string(id)),
	role(role)
{
}
