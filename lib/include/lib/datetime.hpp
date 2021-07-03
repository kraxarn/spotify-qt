#pragma once

#include <string>
#include <iomanip>
#include <sstream>

namespace lib
{
	/**
	 * Date and time
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
		static auto parse(const std::string &value) -> date_time;

		/**
		 * Current date and time in local time
		 * @return Current date
		 */
		static auto now() -> date_time;

		/**
		 * Current date and time in UTC to_string
		 * @return Current UTC date
		 */
		static auto now_utc() -> date_time;

		/**
		 * Seconds since 1970-01-01
		 */
		static auto seconds_since_epoch() -> long;

		/**
		 * If the current instance represents a valid date
		 * @return Date is valid
		 */
		auto is_valid() const -> bool;

		/**
		 * Get locale dependent time
		 * @return Time
		 */
		auto to_time() const -> std::string;

		/**
		 * Get locale dependent date
		 * @return Date
		 */
		auto to_date() const -> std::string;

		/**
		 * Get ISO date (YYYY-MM-DD)
		 * @return Date
		 */
		auto to_iso_date() const -> std::string;

		/**
		 * Get ISO date and time (YYYY-MM-DDTHH:MM:SSZ)
		 * @return Date and time
		 */
		auto to_iso_date_time() const -> std::string;

		/**
		 * Second, 0-60
		 */
		auto get_second() const -> int;

		/**
		 * Minute, 0-59
		 */
		auto get_minute() const -> int;

		/**
		 * Hour, 0-23
		 */
		auto get_hour() const -> int;

		/**
		 * Day in month, 1-31
		 */
		auto get_day() const -> int;

		/**
		 * Month, 1-12
		 */
		auto get_month() const -> int;

		/**
		 * Full year, 1900-
		 */
		auto get_year() const -> int;

	private:
		/**
		 * Underlying structure starts year at 1900
		 */
		static constexpr int c_year_offset = 1900;

		/**
		 * Parse a string as a date using the specified to_string
		 * @param value Date to parse
		 * @param format Expected to_string
		 */
		void parse(const std::string &value, const char *format);

		/**
		 * Format a date as a string using the specified to_string
		 * @param format Format to use
		 * @return Formatted date as string, or an empty string
		 */
		auto format(const char *format) const -> std::string;

		/**
		 * Internal date structure
		 */
		std::tm tm = {0};
	};
}
