#include "strings.hpp"

std::string strings::join(const std::vector<std::string> &strings, const char *separator)
{
	if (strings.empty())
		return std::string();

	return std::accumulate(std::next(strings.begin()), strings.end(), strings[0],
		[separator](const std::string &str1, const std::string &str2)
	{
		return str1 + separator + str2;
	});
}
