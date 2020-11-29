#include "log.hpp"

using namespace lib;

std::vector<LogMessage> log::messages = std::vector<LogMessage>();

void log::message(LogType logType, const std::string &message)
{
	LogMessage msg(logType, message);
	messages.push_back(msg);

	if (logType == LogType::Information)
		std::cout << msg.format() << std::endl;
	else
		std::cerr << msg.format() << std::endl;
}

const std::vector<LogMessage> &log::getMessages()
{
	return messages;
}
