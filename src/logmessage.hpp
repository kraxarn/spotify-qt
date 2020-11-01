#pragma once

#include "enum/logtype.hpp"

#include <QTime>
#include <QLocale>

class LogMessage
{
public:
	LogMessage(const QTime &time, LogType logType, const QString &message);
	LogMessage(LogType logType, const QString &message);

	std::string format();
	QString logTypeString();

	QTime getTime() const;
	LogType getType() const;
	QString getMessage() const;

private:
	QTime time;
	LogType logType;
	QString message;
};