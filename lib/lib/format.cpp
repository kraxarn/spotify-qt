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
