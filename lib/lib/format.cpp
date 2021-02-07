#include "format.hpp"

using namespace lib;

std::string fmt::time(int ms)
{
	auto seconds = ms / 1000;

	auto m = seconds / 60;
	auto s = seconds % 60;

	return format("{}:{}", m,
		format("{}{}", s < 10 ? "0" : "", s % 60));
}

std::string fmt::size(unsigned int bytes)
{
	if (bytes >= 1000000000)
		return format("{} GB", bytes / 1000000000);

	if (bytes >= 1000000)
		return format("{} MB", bytes / 1000000);

	if (bytes >= 1000)
		return format("{} kB", bytes / 1000);

	return format("{} B", bytes);
}
