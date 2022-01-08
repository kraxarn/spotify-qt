#include "listitem/library.hpp"

#include <utility>

ListItem::Library::Library(lib::spt::entity entity, std::string tooltip, DataRole role)
	: entity(std::move(entity)),
	tooltip(std::move(tooltip)),
	role(role)
{
}

ListItem::Library::Library(const lib::spt::entity &entity, DataRole role)
	: Library(entity, entity.name, role)
{
}

auto ListItem::Library::getId() const -> QString
{
	return QString::fromStdString(entity.id);
}

auto ListItem::Library::getName() const -> QString
{
	return QString::fromStdString(getNameString());
}

auto ListItem::Library::getTooltip() const -> QString
{
	return QString::fromStdString(tooltip);
}

auto ListItem::Library::getRole() const -> int
{
	return static_cast<int>(role);
}

auto ListItem::Library::getNameString() const -> const std::string &
{
	return entity.name;
}
