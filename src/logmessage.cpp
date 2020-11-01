#include "logmessage.hpp"

LogMessage::LogMessage(const QTime &time, LogType logType, const QString &message)
	: time(time), logType(logType), message(QString(message))
{
}

LogMessage::LogMessage(LogType logType, const QString &message)
	: LogMessage(QDateTime::currentDateTime().time(), logType, message)
{
}

std::string LogMessage::format()
{
	return QString("[%1] [%2] %3")
		.arg(QLocale::system().toString(QDateTime::currentDateTime().time(), QLocale::ShortFormat))
		.arg(logTypeString())
		.arg(message)
		.toStdString();
}

QString LogMessage::logTypeString()
{
	switch (logType)
	{
		case LogType::Information:
			return "info";

		case LogType::Warning:
			return "warn";

		case LogType::Error:
			return "err ";
	}

	return QString();
}

QTime LogMessage::getTime() const
{
	return time;
}

LogType LogMessage::getType() const
{
	return logType;
}

QString LogMessage::getMessage() const
{
	return message;
}
