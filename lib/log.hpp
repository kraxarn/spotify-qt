#pragma once

#include "enum/logtype.hpp"
#include "log_message.hpp"

#include <iostream>
#include <regex>

namespace lib
{
	/**
	 * Application log
	 */
	class log
	{
	public:
		/**
		 * Log info message with arguments
		 * @param fmt Message with {} as arguments
		 * @param arg Argument
		 * @param args Arguments
		 */
		template<typename Format, typename Arg, typename... Args>
		static void info(const Format &fmt, const Arg &arg, Args &&... args)
		{
			return info(fmt::format(fmt, arg), args...);
		}

		/**
		 * Log info message
		 * @param fmt Message
		 */
		template<typename Format>
		static void info(const Format &fmt)
		{
			message(LogType::Information, fmt);
		}

		/**
		 * Log warning message with arguments
		 * @param fmt Message with {} as arguments
		 * @param arg Argument
		 * @param args Arguments
		 */
		template<typename Format, typename Arg, typename... Args>
		static void warn(const Format &fmt, const Arg &arg, Args &&... args)
		{
			return warn(fmt::format(fmt, arg), args...);
		}

		/**
		 * Log warning message
		 * @param fmt Message
		 */
		template<typename Format>
		static void warn(const Format &fmt)
		{
			message(LogType::Warning, fmt);
		}

		/**
		 * Log error message with {} as arguments
		 * @param fmt Message
		 * @param arg Argument
		 * @param args Arguments
		 */
		template<typename Format, typename Arg, typename... Args>
		static void error(const Format &fmt, const Arg &arg, Args &&... args)
		{
			return error(fmt::format(fmt, arg), args...);
		}

		/**
		 * Log error message
		 * @param fmt Message
		 */
		template<typename Format>
		static void error(const Format &fmt)
		{
			message(LogType::Error, fmt);
		}

		/**
		 * Get all messages that has been logged since application start
		 * @return Messages
		 */
		static const std::vector<log_message> &get_messages();

	private:
		/**
		 * Empty constructor
		 * @note log is static and shouldn't be instanced
		 */
		log() = default;

		/**
		 * Messages logged since application start
		 */
		static std::vector<log_message> messages;

		/**
		 * Log message
		 * @param logType Type of message
		 * @param message Message
		 */
		static void message(LogType logType, const std::string &message);
	};
}