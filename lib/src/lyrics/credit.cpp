#include "lib/lyrics/credit.hpp"
#include "lib/strings.hpp"
#include "lib/log.hpp"

lib::lrc::credit::credit(const std::string &line)
{
	const auto split = line.rfind(':');
	if (split == std::string::npos)
	{
		lib::log::debug("Failed to parse credit \"{}\"", line);
		return;
	}

	name = line.substr(split + 1);
	lib::strings::trim(name);
}

auto lib::lrc::credit::operator<(const lib::lrc::credit &rhs) const -> bool
{
	return name < rhs.name;
}

auto lib::lrc::credit::operator==(const lib::lrc::credit &rhs) const -> bool
{
	return name == rhs.name;
}
