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

std::string strings::trim(std::string &str)
{
	trim_begin(str);
	trim_end(str);

	return str;
}

void strings::trim_begin(std::string &str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char chr)
	{
		return !std::isspace(chr);
	}));
}

void strings::trim_end(std::string &str)
{
	str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char chr)
	{
		return !std::isspace(chr);
	}).base(), str.end());
}

std::vector<std::string> strings::split(const std::string &str, char delimiter)
{
	std::vector<std::string> vec;
	std::istringstream stream(str);
	std::string temp;

	while (std::getline(stream, temp, delimiter))
		vec.push_back(temp);

	return vec;
}

bool strings::starts_with(const std::string &str, const std::string &start)
{
	return str.find(start, 0) == 0;
}
