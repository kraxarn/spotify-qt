#pragma once

#include "lib/datetime.hpp"
#include "lib/enum/logtype.hpp"
#include "lib/format.hpp"

namespace lib
{
	/**
	 * A message in the log
	 */
	class log_message
	{
	public:
		/**
		 * Save a logged message for the specified time
		 * @param time Time it was logged
		 * @param log_type Type of log
		 * @param message Message logged
		 */
		log_message(const date_time &time, log_type log_type,
			const std::string &message);

		/**
		 * Save a logged message for the current time
		 * @param log_type Type of log
		 * @param message Message logged
		 */
		log_message(log_type log_type, const std::string &message);

		/**
		 * Format message as [time] [type] message
		 * @return Formatted message
		 */
		std::string to_string() const;

		/**
		 * log_type as 4 character string
		 * @return Type of log
		 */
		std::string get_type_short() const;

		/**
		 * Get the time the message was logged at
		 * @return Time as locale formatted string
		 */
		std::string get_time() const;

		/**
		 * log_type as a full string
		 * @return Type of log
		 */
		std::string get_type() const;

		/**
		 * Get logged message
		 * @return Message
		 */
		std::string get_message() const;

	private:
		/**
		 * Logged time
		 */
		date_time time;

		/**
		 * Type of log
		 */
		log_type logType;

		/**
		 * Message logged
		 */
		std::string message;
	};
}
