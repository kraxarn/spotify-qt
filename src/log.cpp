#include "log.hpp"

QList<LogMessage> Log::messages = QList<LogMessage>();

void Log::log(LogType logType, const QString &message)
{
	LogMessage msg(logType, message);
	messages.append(msg);

	if (logType == LogType::Information)
		std::cout << msg.format() << std::endl;
	else
		std::cerr << msg.format() << std::endl;
}

void Log::info(const QString &message)
{
	log(LogType::Information, message);
}

void Log::warn(const QString &message)
{
	log(LogType::Warning, message);
}

void Log::error(const QString &message)
{
	log(LogType::Error, message);
}

const QList<LogMessage> &Log::getMessages()
{
	return messages;
}
