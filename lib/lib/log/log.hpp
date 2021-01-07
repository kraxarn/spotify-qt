#pragma once

#include "lib/enum/logtype.hpp"
#include "lib/format.hpp"
#include "lib/log/logmessage.hpp"

#include <iostream>
#include <regex>

namespace lib
{
	/**
	 * Application logging
	 */
	class log
	{
	public:
		/**
		 * Log information with formatting
		 */
		template<typename Format, typename Arg, typename... Args>
		static void info(const Format &fmt, const Arg &arg, Args &&... args)
		{
			return info(fmt::format(fmt, arg, args...));
		}

		/**
		 * Log information
		 */
		template<typename Format>
		static void info(const Format &fmt)
		{
			message(log_type::information, fmt);
		}

		/**
		 * Log warning with formatting
		 */
		template<typename Format, typename Arg, typename... Args>
		static void warn(const Format &fmt, const Arg &arg, Args &&... args)
		{
			return warn(fmt::format(fmt, arg, args...));
		}

		/**
		 * Log warning
		 */
		template<typename Format>
		static void warn(const Format &fmt)
		{
			message(log_type::warning, fmt);
		}

		/***
		 * Log error with formatting
		 */
		template<typename Format, typename Arg, typename... Args>
		static void error(const Format &fmt, const Arg &arg, Args &&... args)
		{
			return error(fmt::format(fmt, arg, args...));
		}

		/**
		 * Log error
		 */
		template<typename Format>
		static void error(const Format &fmt)
		{
			message(log_type::error, fmt);
		}

		/**
		 * Get all messages that has been logged since application start
		 * @return Log messages
		 */
		static const std::vector<log_message> &get_messages();

	private:
		/**
		 * Private constructor, this is a static class
		 */
		log() = default;

		/**
		 * History of all messages
		 */
		static std::vector<log_message> messages;

		/**
		 * Log a message with the specified type
		 * @param logType Type of log
		 * @param message Message to log
		 */
		static void message(log_type log_type, const std::string &message);
	};
}
