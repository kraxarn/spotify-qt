#pragma once

#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>

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

	/**
	 * Trim start and end of any whitespace
	 * @param str String to trim
	 * @remark Argument is modified, return value is the same as the argument
	 * @return Trimmed string
	 */
	static std::string trim(std::string &str);

	/**
	 * Split a string into a vector
	 * @param str String to split
	 * @param delimiter Delimiter to split by
	 * @return Split string
	 */
	static std::vector<std::string> split(const std::string &str, char delimiter);

	/**
	 * Check if a string starts with a specific string
	 * @param str String to check
	 * @param start String that the string should start with
	 * @return String starts with the specified string
	 */
	static bool starts_with(const std::string &str, const std::string &start);

private:
	/**
	 * Trim beginning of string
	 * @param str String to trim
	 */
	static void trim_begin(std::string &str);

	/**
	 * Trim end of string
	 * @param str String to trim
	 */
	static void trim_end(std::string &str);
};