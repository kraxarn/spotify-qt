#pragma once

#include "enum/logtype.hpp"
#include "log_message.hpp"

#include <iostream>
#include <regex>

namespace lib
{
	class log
	{
	public:
		template<typename Format, typename Arg, typename... Args>
		static void info(const Format &fmt, const Arg &arg, Args &&... args)
		{
			return info(fmt::format(fmt, arg), args...);
		}
		template<typename Format>
		static void info(const Format &fmt)
		{
			message(LogType::Information, fmt);
		}

		template<typename Format, typename Arg, typename... Args>
		static void warn(const Format &fmt, const Arg &arg, Args &&... args)
		{
			return warn(fmt::format(fmt, arg), args...);
		}
		template<typename Format>
		static void warn(const Format &fmt)
		{
			message(LogType::Warning, fmt);
		}

		template<typename Format, typename Arg, typename... Args>
		static void error(const Format &fmt, const Arg &arg, Args &&... args)
		{
			return error(fmt::format(fmt, arg), args...);
		}
		template<typename Format>
		static void error(const Format &fmt)
		{
			message(LogType::Error, fmt);
		}

		static const std::vector<LogMessage> &getMessages();

	private:
		log() = default;

		static std::vector<LogMessage> messages;

		static void message(LogType logType, const std::string &message);
	};
}