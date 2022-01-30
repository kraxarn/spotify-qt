#include "lib/logmessage.hpp"
#include "lib/fmt.hpp"

lib::log_message::log_message(const date_time &time, log_type log_type,
	const std::string &message)
	: time(time),
	logType(log_type),
	message(std::string(message))
{
}

lib::log_message::log_message(log_type log_type, const std::string &message)
	: log_message(date_time::now(), log_type, message)
{
}

auto lib::log_message::to_string() const -> std::string
{
	return lib::fmt::format("[{}] [{}] {}", get_time(),
		get_type_short(), message);
}

auto lib::log_message::get_type_short() const -> std::string
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
			return "dbg ";
	}

	return {};
}

auto lib::log_message::get_time() const -> std::string
{
	return time.to_time();
}

auto lib::log_message::get_type() const -> std::string
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
			return "Debug";
	}

	return {};
}

auto lib::log_message::get_message() const -> std::string
{
	return message;
}
