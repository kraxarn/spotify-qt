#pragma once

#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iostream>

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
		static auto join(const std::vector<std::string> &strings,
			const char *separator) -> std::string;

		/**
		 * Check if a string is contained within another string
		 * @param str String to search in
		 * @param value String to search for
		 * @return String contains value
		 */
		template<typename String>
		static auto contains(const std::string &str, const String &value) -> bool
		{
			return str.find(value) != std::string::npos;
		}

		/**
		 * Trim start and end of any whitespace
		 * @param str String to trim
		 * @remark Argument is modified, return value is the same as the argument
		 * @return Trimmed string
		 */
		static auto trim(std::string &str) -> std::string;

		/**
		 * Split a string into a vector
		 * @param str String to split
		 * @param delimiter Delimiter to split by
		 * @return Split string
		 */
		static auto split(const std::string &str,
			const std::string &delimiter) -> std::vector<std::string>;

		/**
		 * Split a string into a vector
		 * @param str String to split
		 * @param delimiter Delimiter to split by
		 * @return Split string
		 */
		static auto split(const std::string &str,
			char delimiter) -> std::vector<std::string>;

		/**
		 * Check if a string starts with a specific string
		 * @param str String to check
		 * @param start String that the string should start with
		 * @return String starts with the specified string
		 */
		static auto starts_with(const std::string &str, const std::string &start) -> bool;

		/**
		 * Remove specified characters from a string
		 * @param str String to remove from
		 * @param substr String to remove
		 * @return Same as str
		 */
		static auto remove(std::string &str, const std::string &substr) -> std::string;

		/**
		 * Check if a string ends with another string
		 * @param str String to check
		 * @param end String it should end with
		 * @return str ends with end
		 */
		static auto ends_with(const std::string &str, const std::string &end) -> bool;

		/**
		 * Try to parse the specified string as an int
		 * @param str String to parse
		 * @param value Value to write the result to
		 * @return If conversion was successful
		 */
		static auto try_to_int(const std::string &str, int &value) -> bool;

		/**
		 * Get the n-leftmost characters of string
		 * @param str String to get from
		 * @param n Number of characters to get
		 * @return Substring of string with specified characters
		 */
		static auto left(const std::string &str, size_t n) -> std::string;

		/**
		 * Get the n-rightmost characters of string
		 * @param str String to get from
		 * @param n Number of characters to get
		 * @return Substring of string with specified characters
		 */
		static auto right(const std::string &str, size_t n) -> std::string;

		/**
		 * Get string as all lowercase
		 * @param str String to transform
		 * @return String is all lowercase, same as str
		 */
		static auto to_lower(const std::string &str) -> std::string;

		/**
		 * Get string as all uppercase
		 * @param str String to transform
		 * @return String in all uppercase
		 */
		static auto to_upper(const std::string &str) -> std::string;

		/**
		 * Capitalize string, first letter in uppercase, and the rest in lowercase
		 * @param str String to transform
		 */
		static auto capitalize(const std::string &str) -> std::string;

		/**
		 * Parse value as string with specified precision
		 * @param value Value to stringify
		 * @param precision Precision (number of digits)
		 * @note Rounds
		 */
		template<typename T>
		static auto to_string(const T value, const int precision) -> std::string
		{
			std::ostringstream stream;
			stream.precision(precision);
			stream << std::fixed << value;
			return stream.str();
		}

		/**
		 * Replace all occurrences of a value in a string
		 * @param str String to replace in
		 * @param old_val Old value
		 * @param new_val New value
		 * @return New string with value replaced
		 */
		static auto replace_all(const std::string &str,
			char old_val, char new_val) -> std::string;

		/**
		 * Replace all occurrences of a string in another string
		 * @param str String to replace in
		 * @param old_val Old string
		 * @param new_val New string
		 * @return New string with string replaced
		 */
		static auto replace_all(const std::string &str,
			const std::string &old_val, const std::string &new_val) -> std::string;

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
