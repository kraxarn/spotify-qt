#include "lib/format.hpp"
#include "lib/fmt.hpp"
#include "lib/time.hpp"

auto lib::format::time(int milliseconds) -> std::string
{
	const auto total_seconds = milliseconds / lib::time::ms_in_sec;

	const auto minutes = total_seconds / lib::time::secs_in_min;
	const auto seconds = total_seconds % lib::time::secs_in_min;

	const auto seconds_prefixed = lib::fmt::format("{}{}",
		seconds < 10 ? "0" : "", seconds);

	return lib::fmt::format("{}:{}", minutes, seconds_prefixed);
}

auto lib::format::size(unsigned long bytes) -> std::string
{
	if (bytes >= giga)
	{
		return lib::fmt::format("{} GB", bytes / giga);
	}

	if (bytes >= mega)
	{
		return lib::fmt::format("{} MB", bytes / mega);
	}

	if (bytes >= kilo)
	{
		return lib::fmt::format("{} kB", bytes / kilo);
	}

	return lib::fmt::format("{} B", bytes);
}

auto lib::format::count(unsigned int count) -> std::string
{
	if (count >= mega)
	{
		return lib::fmt::format("{}M", count / mega);
	}

	if (count >= kilo)
	{
		return lib::fmt::format("{}k", count / kilo);
	}

	return lib::fmt::format("{}", count);
}
