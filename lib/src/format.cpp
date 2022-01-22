#include "lib/format.hpp"

auto lib::fmt::time(int ms) -> std::string
{
	auto seconds = ms / msInSec;

	auto m = seconds / secsInMin;
	auto s = seconds % secsInMin;

	return format("{}:{}", m,
		format("{}{}", s < 10 ? "0" : "", s % secsInMin));
}

auto lib::fmt::size(unsigned int bytes) -> std::string
{
	if (bytes >= giga)
	{
		return format("{} GB", bytes / giga);
	}

	if (bytes >= mega)
	{
		return format("{} MB", bytes / mega);
	}

	if (bytes >= kilo)
	{
		return format("{} kB", bytes / kilo);
	}

	return format("{} B", bytes);
}

auto lib::fmt::count(unsigned int count) -> std::string
{
	if (count >= mega)
	{
		return format("{}M", count / mega);
	}

	if (count >= kilo)
	{
		return format("{}k", count / kilo);
	}

	return format("{}", count);
}
