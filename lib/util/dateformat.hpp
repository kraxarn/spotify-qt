#pragma once

#include <iomanip>
#include <string>
#include <sstream>

namespace lib
{
	/**
	 * Basic date formatter and parser
	 */
	class DateFormat
	{
	public:
		enum class Format
		{
			/**
			 * Locale dependent time format (HH:MM for example)
			 */
			LocalTime,
			/**
			 * Locale dependent date format (YYYY-MM-DD for example)
			 */
			LocalDate,
			/**
			 * ISO 8601 date (YYYY-MM-DD)
			 */
			IsoDate,
			/**
			 * ISO 8601 date and time (YYYY-MM-DDTHH:MM:SSZ)
			 */
			IsoDateTime
		};

		/**
		 * Format date as a string
		 * @param format Format to use
		 * @param date Date to format
		 * @return Formatted string
		 */
		static std::string format(Format format, std::tm *date);

		/**
		 * Parse a date
		 * @param value Date to parse
		 * @param format Format of date to parse
		 * @param date Parsed date
		 */
		static void parse(const std::string &value, Format format, std::tm *date);

	private:
		/**
		 * Date format enum to string
		 * @param format Date format
		 * @return Format string
		 */
		static const char *formatString(Format format);
	};
}