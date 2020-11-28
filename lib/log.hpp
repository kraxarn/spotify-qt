#pragma once

#include "enum/logtype.hpp"
#include "logmessage.hpp"

#include <iostream>
#include <regex>

namespace lib
{
	class Log
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
			log(LogType::Information, fmt);
		}

		template<typename Format, typename Arg, typename... Args>
		static void warn(const Format &fmt, const Arg &arg, Args &&... args)
		{
			return warn(fmt::format(fmt, arg), args...);
		}
		template<typename Format>
		static void warn(const Format &fmt)
		{
			log(LogType::Warning, fmt);
		}

		template<typename Format, typename Arg, typename... Args>
		static void error(const Format &fmt, const Arg &arg, Args &&... args)
		{
			return error(fmt::format(fmt, arg), args...);
		}
		template<typename Format>
		static void error(const Format &fmt)
		{
			log(LogType::Error, fmt);
		}

		static const std::vector<LogMessage> &getMessages();

	private:
		Log() = default;

		static std::vector<LogMessage> messages;

		static void log(LogType logType, const std::string &message);
	};
}