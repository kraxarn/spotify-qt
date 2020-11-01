#pragma once

#include "enum/logtype.hpp"
#include "util/utils.hpp"
#include "logmessage.hpp"

#include <iostream>

#include <QString>

class Log
{
public:
	template<typename... Args>
	static void info(const QString &message, Args&... args)
	{
		info(collect(message, args...));
	}
	static void info(const QString &message);

	template<typename ... Args>
	static void warn(const QString &message, Args&... args)
	{
		warn(collect(message, args...));
	}
	static void warn(const QString &message);

	template<typename ... Args>
	static void error(const QString &message, Args&... args)
	{
		error(collect(message, args...));
	}
	static void error(const QString &message);

	static const QList<LogMessage> &getMessages();

private:
	Log() = default;

	static QList<LogMessage> messages;

	static void log(LogType logType, const QString &message);

	template<typename T>
	static QString collect(const QString &message, T &arg)
	{
		return QString(message).arg(arg);
	}
};