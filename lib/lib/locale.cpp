#include "locale.hpp"

bool lib::locale::set(const std::string &locale, bool force)
{
	if (std::locale().name() == "C" && !force)
		return true;

	try
	{
		std::locale::global(std::locale(strings::ends_with(locale, ".UTF-8")
			? locale
			: fmt::format("{}.UTF-8", locale)));
		return true;
	}
	catch (const std::exception &e)
	{
		return false;
	}
}
