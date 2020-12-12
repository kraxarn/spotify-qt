#pragma once

#include "log.hpp"

#include <string>
#include <iomanip>
#include <sstream>

namespace lib
{
	/**
	 * Date and time
	 */
	class date
	{
	public:
		/**
		 * Construct a new empty and invalid date
		 */
		date() = default;

		/**
		 * Construct a date from another date
		 * @param date A new copy of date
		 */
		date(const date &date);

		/**
		 * Try to parse a date from a string
		 * @param value ISO date
		 * @return A date, invalid if parsing failed
		 */
		static date parse(const std::string &value);

		/**
		 * Current date and time in local time
		 * @return Current date
		 */
		static date now();

		/**
		 * Current date and time in UTC format
		 * @return Current UTC date
		 */
		static date now_utc();

		/**
		 * If the current instance represents a valid date
		 * @return Date is valid
		 */
		bool is_valid() const;

		/**
		 * Date is 1970-01-01 or invalid
		 * @return Date is empty
		 */
		bool is_empty() const;

		/**
		 * Get locale dependent time
		 * @return Time
		 */
		std::string to_time() const;

		/**
		 * Get locale dependent date
		 * @return Date
		 */
		std::string to_date() const;

		/**
		 * Get ISO date (YYYY-MM-DD)
		 * @return Date
		 */
		std::string to_iso_date() const;

		/**
		 * Get ISO date and time (YYYY-MM-DDTHH:MM:SSZ)
		 * @return Date and time
		 */
		std::string to_iso_date_time() const;

	private:
		/**
		 * Parse a string as a date using the specified format
		 * @param value Date to parse
		 * @param format Expected format
		 */
		void parse(const std::string &value, const char* format);

		/**
		 * Format a date as a string using the specified format
		 * @param format Format to use
		 * @return Formatted date as string, or an empty string
		 */
		std::string format(const char* format) const;

		/**
		 * Internal date structure
		 */
		std::tm *tm = nullptr;
	};
}
