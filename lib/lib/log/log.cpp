#include "log.hpp"

using namespace lib;

std::vector<log_message> log::messages = std::vector<log_message>();

bool log::log_to_stdout = true;

void log::message(log_type log_type, const std::string &message)
{
	log_message msg(log_type, message);
	messages.push_back(msg);

	if (!log_to_stdout)
		return;

	if (log_type == log_type::information || log_type == log_type::verbose)
		std::cout << msg.to_string() << std::endl;
	else
		std::cerr << msg.to_string() << std::endl;
}

const std::vector<log_message> &log::get_messages()
{
	return messages;
}

void log::clear()
{
	messages.clear();
}

void log::set_log_to_stdout(bool value)
{
	log_to_stdout = value;
}
