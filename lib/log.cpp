#include "log.hpp"

using namespace lib;

std::vector<log_message> log::messages = std::vector<log_message>();

void log::message(LogType logType, const std::string &message)
{
	log_message msg(logType, message);
	messages.push_back(msg);

	if (logType == LogType::Information)
		std::cout << msg.format() << std::endl;
	else
		std::cerr << msg.format() << std::endl;
}

const std::vector<log_message> &log::get_messages()
{
	return messages;
}
