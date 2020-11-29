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
};