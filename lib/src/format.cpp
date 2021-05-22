#include "lib/format.hpp"

using namespace lib;

auto fmt::time(int ms) -> std::string
{
	auto seconds = ms / size_k;

	auto m = seconds / 60;
	auto s = seconds % 60;

	return format("{}:{}", m,
		format("{}{}", s < 10 ? "0" : "", s % 60));
}

auto fmt::size(unsigned int bytes) -> std::string
{
	if (bytes >= size_g)
	{
		return format("{} GB", bytes / size_g);
	}

	if (bytes >= size_m)
	{
		return format("{} MB", bytes / size_m);
	}

	if (bytes >= size_k)
	{
		return format("{} kB", bytes / size_k);
	}

	return format("{} B", bytes);
}

auto fmt::count(unsigned int count) -> std::string
{
	if (count >= size_m)
	{
		return format("{}M", count / size_m);
	}

	if (count >= size_k)
	{
		return format("{}k", count / size_k);
	}

	return format("{}", count);
}
