#pragma once

#include "enum/logtype.hpp"
#include "util/utils.hpp"
#include "logmessage.hpp"

#include <iostream>

#include <QString>

class Log
{
public:
	static void info(const QString &message);
	static void warn(const QString &message);
	static void error(const QString &message);

private:
	Log() = default;

	static QList<LogMessage> messages;

	static void log(LogType logType, const QString &message);
};