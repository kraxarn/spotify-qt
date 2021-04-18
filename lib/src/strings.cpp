#include "lib/strings.hpp"

using namespace lib;

auto strings::join(const std::vector<std::string> &strings, const char *separator) -> std::string
{
	if (strings.empty())
	{
		return std::string();
	}

	return std::accumulate(std::next(strings.begin()), strings.end(), strings[0],
		[separator](const std::string &str1, const std::string &str2) -> std::string
		{
			return str1 + separator + str2;
		});
}

auto strings::trim(std::string &str) -> std::string
{
	trim_begin(str);
	trim_end(str);

	return str;
}

void strings::trim_begin(std::string &str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), []
		(unsigned char chr) -> bool
	{
		return std::isspace(chr) == 0;
	}));
}

void strings::trim_end(std::string &str)
{
	str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char chr) -> bool
	{
		return std::isspace(chr) == 0;
	}).base(), str.end());
}

auto strings::split(const std::string &str,
	const char *delimiter) -> std::vector<std::string>
{
	std::stringstream stream(str);
	std::istream_iterator<std::string> begin(stream);
	std::istream_iterator<std::string> end;
	std::vector<std::string> vec(begin, end);

	std::copy(vec.begin(), vec.end(),
		std::ostream_iterator<std::string>(std::cout, delimiter));

	if (vec.empty())
	{
		vec.push_back(str);
	}

	return vec;
}

auto strings::starts_with(const std::string &str, const std::string &start) -> bool
{
	return str.find(start, 0) == 0;
}

auto strings::remove(std::string &str, const std::string &substr) -> std::string
{
	auto pos = str.find(substr);
	if (pos != std::string::npos)
	{
		str.erase(pos, substr.length());
	}
	return str;
}

auto strings::ends_with(const std::string &str, const std::string &end) -> bool
{
	return str.length() >= end.length()
		&& str.compare(str.length() - end.length(), end.length(), end) == 0;
}

auto strings::try_to_int(const std::string &str, int &value) -> bool
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

auto strings::left(const std::string &str, size_t n) -> std::string
{
	return str.length() < n
		? str
		: str.substr(0, n);
}

auto strings::right(const std::string &str, size_t n) -> std::string
{
	return str.length() < n
		? str
		: str.substr(str.length() - n);
}

auto strings::to_lower(const std::string &str) -> std::string
{
	std::string val(str);
	std::transform(val.begin(), val.end(), val.begin(), [](unsigned char c) -> unsigned char
	{
		return std::tolower(c);
	});
	return val;
}

auto strings::to_upper(const std::string &str) -> std::string
{
	std::string val(str);
	std::transform(val.begin(), val.end(), val.begin(), [](unsigned char c) -> unsigned char
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

auto strings::index_of(const std::string &str, const std::string &keyword) -> unsigned long
{
	auto result = str.find(keyword);
	return result == std::string::npos
		? -1
		: result;
}

auto strings::last_index_of(const std::string &str, const std::string &keyword) -> unsigned long
{
	auto result = str.rfind(keyword);
	return result == std::string::npos
		? -1
		: result;
}
