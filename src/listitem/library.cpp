#include "listitem/library.hpp"
#include "metatypes.hpp"

ListItem::Library::Library(const lib::spt::album &album)
	: entity(QVariant::fromValue(album))
{
}

ListItem::Library::Library(const lib::spt::artist &artist)
	: entity(QVariant::fromValue(artist))
{
}

auto ListItem::Library::name() const -> std::string
{
	if (entity.canConvert<lib::spt::album>())
	{
		return entity.value<lib::spt::album>().name;
	}

	if (entity.canConvert<lib::spt::artist>())
	{
		return entity.value<lib::spt::artist>().name;
	}

	return {};
}

auto ListItem::Library::tooltip() const -> QString
{
	if (entity.canConvert<lib::spt::album>())
	{
		return {};
	}

	return QString::fromStdString(name());
}

auto ListItem::Library::data() const -> const QVariant &
{
	return entity;
}
