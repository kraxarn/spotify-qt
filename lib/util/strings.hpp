#pragma once

#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>

/**
 * String utilities
 */
class strings
{
public:
	/**
	 * Join a vector of strings into a single string
	 * @param strings Strings to join
	 * @param separator Separator between strings
	 * @return Joined string
	 */
	static std::string join(const std::vector<std::string> &strings, const char *separator);

	/**
	 * Check if a string is contained within another string
	 * @param str String to search in
	 * @param value String to search for
	 * @return String contains value
	 */
	template<typename String>
	static bool contains(const std::string &str, const String &value)
	{
		return str.find(value) != std::string::npos;
	}
};