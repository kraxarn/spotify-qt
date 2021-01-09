#pragma once

#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>

namespace lib
{
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

		/**
		 * Remove specified characters from a string
		 * @param str String to remove from
		 * @param substr String to remove
		 * @return Same as str
		 */
		static std::string remove(std::string &str, const std::string &substr);

		/**
		 * Check if a string ends with another string
		 * @param str String to check
		 * @param end String it should end with
		 * @return str ends with end
		 */
		static bool ends_with(const std::string &str, const std::string &end);

		/**
		 * Try to parse the specified string as an int
		 * @param str String to parse
		 * @param value Value to write the result to
		 * @return If conversion was successful
		 */
		static bool try_to_int(const std::string &str, int &value);

		/**
		 * Get the n-leftmost characters of string
		 * @param str String to get from
		 * @param n Number of characters to get
		 * @return Substring of string with specified characters
		 */
		static std::string left(const std::string &str, size_t n);

		/**
		 * Get the n-rightmost characters of string
		 * @param str String to get from
		 * @param n Number of characters to get
		 * @return Substring of string with specified characters
		 */
		static std::string right(const std::string &str, size_t n);

		/**
		 * Get strings as all lowercase
		 * @param str String to transform
		 * @return String is all lowercase, same as str
		 */
		static std::string to_lower(std::string &str);

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
}