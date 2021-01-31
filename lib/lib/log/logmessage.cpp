#include "logmessage.hpp"

using namespace lib;

log_message::log_message(const date_time &time, log_type log_type,
	const std::string &message)
	: time(time),
	logType(log_type),
	message(std::string(message))
{
}

log_message::log_message(log_type log_type, const std::string &message)
	: log_message(date_time::now(), log_type, message)
{
}

std::string log_message::to_string() const
{
	return fmt::format("[{}] [{}] {}", get_time(),
		get_type_short(), message);
}

std::string log_message::get_type_short() const
{
	switch (logType)
	{
		case log_type::information:
			return "info";

		case log_type::warning:
			return "warn";

		case log_type::error:
			return "err ";

		case log_type::verbose:
			return "dev ";
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
		case log_type::information:
			return "Information";

		case log_type::warning:
			return "Warning";

		case log_type::error:
			return "Error";

		case log_type::verbose:
			return "Developer";
	}

	return std::string();
}

std::string log_message::get_message() const
{
	return message;
}
