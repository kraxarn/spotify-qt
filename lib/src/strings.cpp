#include "lib/strings.hpp"

auto lib::strings::join(const std::vector<std::string> &strings,
	const char *separator) -> std::string
{
	if (strings.empty())
	{
		return {};
	}

	return std::accumulate(std::next(strings.begin()), strings.end(), strings[0],
		[separator](const std::string &str1, const std::string &str2) -> std::string
		{
			return str1 + separator + str2;
		});
}

auto lib::strings::trim(std::string &str) -> std::string
{
	trim_begin(str);
	trim_end(str);

	return str;
}

void lib::strings::trim_begin(std::string &str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(),
		[](unsigned char chr) -> bool
		{
			return std::isspace(chr) == 0;
		}));
}

void lib::strings::trim_end(std::string &str)
{
	str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char chr) -> bool
	{
		return std::isspace(chr) == 0;
	}).base(), str.end());
}

auto lib::strings::split(const std::string &str,
	const std::string &delimiter) -> std::vector<std::string>
{
	std::vector<std::string> vec;
	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != std::string::npos)
	{
		vec.push_back(str.substr(start, end - start));
		start = end + delimiter.size();
		end = str.find(delimiter, start);
	}

	if (start != end)
	{
		vec.push_back(str.substr(start, end));
	}
	else if (vec.empty())
	{
		vec.push_back(str);
	}

	return vec;
}

auto lib::strings::split(const std::string &str, char delimiter) -> std::vector<std::string>
{
	std::vector<std::string> vec;
	std::istringstream stream(str);
	std::string temp;

	while (std::getline(stream, temp, delimiter))
	{
		vec.push_back(temp);
	}

	if (vec.empty())
	{
		vec.push_back(str);
	}

	return vec;
}

auto lib::strings::starts_with(const std::string &str, const std::string &start) -> bool
{
	return str.find(start, 0) == 0;
}

auto lib::strings::remove(std::string &str, const std::string &substr) -> std::string
{
	auto pos = str.find(substr);
	if (pos != std::string::npos)
	{
		str.erase(pos, substr.length());
	}
	return str;
}

auto lib::strings::ends_with(const std::string &str, const std::string &end) -> bool
{
	return str.length() >= end.length()
		&& str.compare(str.length() - end.length(), end.length(), end) == 0;
}

auto lib::strings::try_to_int(const std::string &str, int &value) -> bool
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

auto lib::strings::left(const std::string &str, size_t n) -> std::string
{
	return str.length() < n
		? str
		: str.substr(0, n);
}

auto lib::strings::right(const std::string &str, size_t n) -> std::string
{
	return str.length() < n
		? str
		: str.substr(str.length() - n);
}

auto lib::strings::to_lower(const std::string &str) -> std::string
{
	std::string val(str);
	std::transform(val.begin(), val.end(), val.begin(), [](unsigned char c) -> unsigned char
	{
		return std::tolower(c);
	});
	return val;
}

auto lib::strings::to_upper(const std::string &str) -> std::string
{
	std::string val(str);
	std::transform(val.begin(), val.end(), val.begin(), [](unsigned char c) -> unsigned char
	{
		return std::toupper(c);
	});
	return val;
}

auto lib::strings::capitalize(const std::string &str) -> std::string
{
	return lib::fmt::format("{}{}",
		to_upper(str.substr(0, 1)),
		to_lower(str.substr(1)));
}

auto lib::strings::replace_all(const std::string &str, char old_val, char new_val) -> std::string
{
	std::string val = str;
	std::replace(val.begin(), val.end(), old_val, new_val);
	return val;
}

auto lib::strings::replace_all(const std::string &str, const std::string &old_val,
	const std::string &new_val) -> std::string
{
	std::string val = str;
	size_t pos = val.find(old_val);

	while (pos != std::string::npos)
	{
		val.replace(pos, old_val.size(), new_val);
		pos = val.find(old_val, pos + old_val.size());
	}

	return val;
}
