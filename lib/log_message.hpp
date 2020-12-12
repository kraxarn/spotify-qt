#pragma once

#include "enum/logtype.hpp"
#include "format.hpp"
#include "util/date.hpp"

#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace lib
{
	/**
	 * A message logged in the log
	 */
	class log_message
	{
	public:
		/**
		 * Construct a new log message at the specified date
		 * @param time Date of log
		 * @param log_type Type of log
		 * @param message Message logged
		 */
		log_message(const date &time, LogType log_type, const std::string &message);

		/**
		 * Construct a new log message at the current date
		 * @param log_type Type of log
		 * @param message Message logged
		 */
		log_message(LogType log_type, const std::string &message);

		/**
		 * Format log message as [<time>] [<type>] <message>
		 * @return Formatted message
		 */
		std::string format() const;

		/**
		 * Get string version of log type
		 * @return info, warn or err
		 */
		std::string log_type_string() const;

		/**
		 * Get time of log in local format
		 * @return Time of log
		 */
		std::string get_time() const;

		/**
		 * Get type of log as string
		 * @return Information, Warning or Error
		 */
		std::string get_type() const;

		/**
		 * Get the logged message
		 * @return
		 */
		std::string get_message() const;

	private:
		/**
		 * Date of log
		 */
		date time;

		/**
		 * Type of log
		 */
		LogType logType;

		/**
		 * The message logged
		 */
		std::string message;
	};
}