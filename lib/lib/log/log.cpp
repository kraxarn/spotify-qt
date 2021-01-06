#include "log.hpp"

using namespace lib;

std::vector<log_message> log::messages = std::vector<log_message>();

void log::message(log_type log_type, const std::string &message)
{
	log_message msg(log_type, message);
	messages.push_back(msg);

	if (log_type == log_type::information)
		std::cout << msg.to_string() << std::endl;
	else
		std::cerr << msg.to_string() << std::endl;
}

const std::vector<log_message> &log::get_messages()
{
	return messages;
}
