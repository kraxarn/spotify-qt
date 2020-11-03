#pragma once

#include "enum/logtype.hpp"
#include "util/utils.hpp"
#include "logmessage.hpp"

#include <iostream>
#include <regex>

class Log
{
public:
	template<typename Format, typename Arg, typename... Args>
	static void info(const Format &fmt, const Arg &arg, Args&&... args)
	{
		return info(collect(fmt, arg), args...);
	}
	template<typename Format>
	static void info(const Format &fmt)
	{
		log(LogType::Information, fmt);
	}

	template<typename Format, typename Arg, typename... Args>
	static void warn(const Format &fmt, const Arg &arg, Args&&... args)
	{
		return warn(collect(fmt, arg), args...);
	}
	template<typename Format>
	static void warn(const Format &fmt)
	{
		log(LogType::Warning, fmt);
	}

	template<typename Format, typename Arg, typename... Args>
	static void error(const Format &fmt, const Arg &arg, Args&&... args)
	{
		return error(collect(fmt, arg), args...);
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

	template<typename Format>
	static std::string collect(const Format &fmt, const std::string &arg)
	{
		auto str = std::string(fmt);
		auto index = str.find("{}");
		if (index == std::string::npos)
			return str;

		return str.replace(index, 2, arg);
	}

	template<typename Format, typename Arg>
	static std::string collect(const Format &fmt, const Arg &arg)
	{
		return collect(fmt, std::to_string(arg));
	}

	template<typename Format>
	static std::string collect(const Format &fmt, const QString &arg)
	{
		std::string str = arg.toStdString();
		return collect(fmt, str);
	}
};