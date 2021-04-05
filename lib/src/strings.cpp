#include "lib/strings.hpp"

using namespace lib;

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

std::string strings::remove(std::string &str, const std::string &substr)
{
	auto pos = str.find(substr);
	if (pos != std::string::npos)
		str.erase(pos, substr.length());
	return str;
}

bool strings::ends_with(const std::string &str, const std::string &end)
{
	return str.length() < end.length()
		? false
		: str.compare(str.length() - end.length(), end.length(), end) == 0;
}

bool strings::try_to_int(const std::string &str, int &value)
{
	try
	{
		value = std::stoi(str);
		return true;
	}
	catch (std::invalid_argument &)
	{
		return false;
	}
}

std::string strings::left(const std::string &str, size_t n)
{
	return str.length() < n
		? str
		: str.substr(0, n);
}

std::string strings::right(const std::string &str, size_t n)
{
	return str.length() < n
		? str
		: str.substr(str.length() - n);
}

auto strings::to_lower(const std::string &str) -> std::string
{
	std::string val(str);
	std::transform(val.begin(), val.end(), val.begin(), [](unsigned char c)
	{
		return std::tolower(c);
	});
	return val;
}

auto strings::to_upper(const std::string &str) -> std::string
{
	std::string val(str);
	std::transform(val.begin(), val.end(), val.begin(), [](unsigned char c)
	{
		return std::toupper(c);
	});
	return val;
}

auto strings::capitalize(const std::string &str) -> std::string
{
	return lib::fmt::format("{}{}",
		to_upper(str.substr(0, 1)),
		to_lower(str.substr(1)));
}
