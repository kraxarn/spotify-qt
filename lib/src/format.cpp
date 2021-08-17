#include "lib/format.hpp"

using namespace lib;

auto fmt::time(int ms) -> std::string
{
	auto seconds = ms / 1000;

	auto m = seconds / 60;
	auto s = seconds % 60;

	return format("{}:{}", m,
		format("{}{}", s < 10 ? "0" : "", s % 60));
}

auto fmt::size(unsigned int bytes) -> std::string
{
	if (bytes >= 1000000000)
	{
		return format("{} GB", bytes / 1000000000);
	}

	if (bytes >= 1000000)
	{
		return format("{} MB", bytes / 1000000);
	}

	if (bytes >= 1000)
	{
		return format("{} kB", bytes / 1000);
	}

	return format("{} B", bytes);
}

auto fmt::count(unsigned int count) -> std::string
{
	if (count >= 1000000)
	{
		return format("{}M", count / 1000000);
	}

	if (count >= 1000)
	{
		return format("{}k", count / 1000);
	}

	return format("{}", count);
}
