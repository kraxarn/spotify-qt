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

const std::vector<LogMessage> &Log::getMessages()
{
	return messages;
}
