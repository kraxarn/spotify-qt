#include "lib/lyrics/credit.hpp"
#include "lib/strings.hpp"
#include "lib/log.hpp"

lib::lrc::credit::credit(const std::string &line)
{
	auto values = lib::strings::split(line, ':');
	if (values.size() != 2)
	{
		lib::log::debug("Failed to parse credit \"{}\": Unexpected values", line);
		return;
	}

	const auto credit_type = lib::strings::trim(values[0]);
	name = lib::strings::trim(values[1]);
}

lib::lrc::credit::credit(lib::lyrics_credit type, const std::string &name)
	: type(type), name(name)
{
}
