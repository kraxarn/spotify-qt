#include "log_message.hpp"

using namespace lib;

log_message::log_message(const date &time, LogType log_type, const std::string &message)
	: time(time), logType(log_type), message(std::string(message))
{
}

log_message::log_message(LogType log_type, const std::string &message)
	: log_message(date::now(), log_type, message)
{
}

std::string log_message::format() const
{
	return fmt::format("[{}] [{}] {}", get_time(), log_type_string(), message);
}

std::string log_message::log_type_string() const
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

	return std::string();
}

std::string log_message::get_time() const
{
	return time.to_time();
}

std::string log_message::get_type() const
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

	return std::string();
}

std::string log_message::get_message() const
{
	return message;
}
