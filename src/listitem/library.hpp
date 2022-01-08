#pragma once

#include "enum/datarole.hpp"
#include "lib/spotify/entity.hpp"

#include <QString>

namespace ListItem
{
	class Library
	{
	public:
		Library(lib::spt::entity entity, std::string tooltip, DataRole role);
		Library(const lib::spt::entity &entity, DataRole role);

		auto getId() const -> QString;
		auto getName() const -> QString;
		auto getTooltip() const -> QString;
		auto getRole() const -> int;

		auto getNameString() const -> const std::string &;

	private:
		lib::spt::entity entity;
		std::string tooltip;
		DataRole role;
	};
}
