#include "log.hpp"

std::vector<LogMessage> Log::messages = std::vector<LogMessage>();

void Log::log(LogType logType, const std::string &message)
{
	LogMessage msg(logType, message);
	messages.push_back(msg);

	if (logType == LogType::Information)
		std::cout << msg.format() << std::endl;
	else
		std::cerr << msg.format() << std::endl;
}

void Log::info(const std::string &message)
{
	log(LogType::Information, message);
}

void Log::warn(const std::string &message)
{
	log(LogType::Warning, message);
}

void Log::error(const std::string &message)
{
	log(LogType::Error, message);
}

const std::vector<LogMessage> &Log::getMessages()
{
	return messages;
}

std::string Log::collect(const std::string &message, std::string &arg)
{
	return std::regex_replace(message, std::regex("\\{}"), arg);
}

std::string Log::collect(const std::string &message, QString &arg)
{
	std::string str = arg.toStdString();
	return collect(message, str);
}
