#pragma once

#include "lib/enum/logtype.hpp"
#include "lib/format.hpp"
#include "lib/logmessage.hpp"
#include "lib/developermode.hpp"

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
			info(fmt::format(fmt, arg, args...));
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
			warn(fmt::format(fmt, arg, args...));
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
			error(fmt::format(fmt, arg, args...));
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
		 * Log verbose message with formatting
		 * @note developer_mode needs to be enabled
		 */
		template<typename Format, typename Arg, typename... Args>
		static void debug(const Format &fmt, const Arg &arg, Args &&... args)
		{
			debug(fmt::format(fmt, arg, args...));
		}

		/**
		 * Log verbose message
		 * @note developer_mode needs to be enabled
		 */
		template<typename Format>
		static void debug(const Format &fmt)
		{
			if (!developer_mode::enabled)
			{
				return;
			}

			message(log_type::verbose, fmt);
		}

		/**
		 * Get all messages that has been logged since application start
		 * @return Log messages
		 */
		static auto get_messages() -> const std::vector<log_message> &;

		/**
		 * Clears all messages in the log
		 */
		static void clear();

		/**
		 * Enable or disable logging to standard output/error,
		 * enabled by default
		 */
		static void set_log_to_stdout(bool value);

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
		 * Also print to stdout/stderr
		 */
		static bool log_to_stdout;

		/**
		 * Log a message with the specified type
		 * @param logType Type of log
		 * @param message Message to log
		 */
		static void message(log_type log_type, const std::string &message);
	};
}
