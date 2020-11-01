#pragma once

#include "enum/logtype.hpp"
#include "util/utils.hpp"
#include "logmessage.hpp"

#include <iostream>
#include <regex>

class Log
{
public:
	template<typename... Args>
	static void info(const std::string &message, Args... args)
	{
		info(collect(message, args...));
	}
	static void info(const std::string &message);

	template<typename ... Args>
	static void warn(const std::string &message, Args... args)
	{
		warn(collect(message, args...));
	}
	static void warn(const std::string &message);

	template<typename ... Args>
	static void error(const std::string &message, Args... args)
	{
		error(collect(message, args...));
	}
	static void error(const std::string &message);

	static const std::vector<LogMessage> &getMessages();

private:
	Log() = default;

	static std::vector<LogMessage> messages;

	static void log(LogType logType, const std::string &message);

	template<typename T>
	static std::string collect(const std::string &message, T &arg)
	{
		return collect(message, std::to_string(arg));
	}
	static std::string collect(const std::string &message, std::string &arg);
	static std::string collect(const std::string &message, QString &arg);
};