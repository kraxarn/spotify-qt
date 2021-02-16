#pragma once

#include <string>
#include <iomanip>
#include <sstream>

namespace lib
{
	/**
	 * Date and time
	 * @deprecated Unreliable and inconsistent, dates should be represented as ISO dates instead
	 */
	class date_time
	{
	public:
		/**
		 * Construct a new empty and invalid date
		 */
		date_time() = default;

		/**
		 * Construct a new date from a set date
		 * @param year Year, 1900-
		 * @param month Month, 1-12
		 * @param day Day, 1-31
		 * @param hour Hour, 0-23
		 * @param minute Minute, 0-59
		 * @param second Second, 0-60
		 */
		date_time(int year, int month, int day, int hour, int minute, int second);

		/**
		 * Construct a date from another date
		 * @param date A new copy of date
		 */
		date_time(const date_time &date);

		/**
		 * Try to parse a date from a string
		 * @param value ISO date
		 * @return A date, invalid if parsing failed
		 */
		static date_time parse(const std::string &value);

		/**
		 * Current date and time in local time
		 * @return Current date
		 */
		static date_time now();

		/**
		 * Current date and time in UTC to_string
		 * @return Current UTC date
		 */
		static date_time now_utc();

		/**
		 * If the current instance represents a valid date
		 * @return Date is valid
		 */
		bool is_valid() const;

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

		/**
		 * Second, 0-60
		 */
		int get_second() const;

		/**
		 * Minute, 0-59
		 */
		int get_minute() const;

		/**
		 * Hour, 0-23
		 */
		int get_hour() const;

		/**
		 * Day in month, 1-31
		 */
		int get_day() const;

		/**
		 * Month, 1-12
		 */
		int get_month() const;

		/**
		 * Full year, 1900-
		 */
		int get_year() const;

	private:
		/**
		 * Parse a string as a date using the specified to_string
		 * @param value Date to parse
		 * @param format Expected to_string
		 */
		void parse(const std::string &value, const char* format);

		/**
		 * Format a date as a string using the specified to_string
		 * @param format Format to use
		 * @return Formatted date as string, or an empty string
		 */
		std::string format(const char* format) const;

		/**
		 * Internal date structure
		 */
		std::tm tm = {0};
	};
}
