#pragma once

#include "lib/spotify/album.hpp"
#include "lib/spotify/artist.hpp"

#include <QVariant>

namespace ListItem
{
	class Library
	{
	public:
		explicit Library(const lib::spt::album &album);
		explicit Library(const lib::spt::artist &artist);

		auto name() const -> std::string;
		auto tooltip() const -> QString;
		auto data() const -> const QVariant &;

	private:
		QVariant entity;
	};
}
