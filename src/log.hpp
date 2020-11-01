#pragma once

#include "enum/logtype.hpp"
#include "util/utils.hpp"

#include <iostream>

#include <QDebug>
#include <QDateTime>
#include <QLocale>

class Log
{
public:
	static void info(const QString &message);
	static void warn(const QString &message);
	static void error(const QString &message);

private:
	Log() = default;

	static void log(LogType logType, const QString &message);
	static QString logTypeString(LogType logType);
};