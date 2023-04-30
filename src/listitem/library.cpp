#include "listitem/library.hpp"

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
		const auto album = entity.value<lib::spt::album>();

		return QString("%1\nBy %2")
			.arg(QString::fromStdString(album.name),
				QString::fromStdString(album.artist));
	}

	return QString::fromStdString(name());
}

auto ListItem::Library::data() const -> const QVariant &
{
	return entity;
}
