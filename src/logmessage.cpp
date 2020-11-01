#include "logmessage.hpp"

LogMessage::LogMessage(const QTime &time, LogType logType, const std::string &message)
	: time(time), logType(logType), message(std::string(message))
{
}

LogMessage::LogMessage(LogType logType, const std::string &message)
	: LogMessage(QDateTime::currentDateTime().time(), logType, message)
{
}

std::string LogMessage::format() const
{
	return QString("[%1] [%2] %3")
		.arg(getTime())
		.arg(logTypeString())
		.arg(QString::fromStdString(message))
		.toStdString();
}

QString LogMessage::logTypeString() const
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

QString LogMessage::getTime() const
{
	return QLocale::system().toString(time, QLocale::ShortFormat);
}

QString LogMessage::getType() const
{
	switch (logType)
	{
		case LogType::Information:
			return "Information";

		case LogType::Warning:
			return "Warning";

		case LogType::Error:
			return "Error";
	}

	return QString();
}

QString LogMessage::getMessage() const
{
	return QString::fromStdString(message);
}
