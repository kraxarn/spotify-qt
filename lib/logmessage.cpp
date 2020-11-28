#include "logmessage.hpp"

using namespace lib;

LogMessage::LogMessage(const std::time_t &time, LogType logType, const std::string &message)
	: time(time), logType(logType), message(std::string(message))
{
}

LogMessage::LogMessage(LogType logType, const std::string &message)
	: LogMessage(std::time(nullptr), logType, message)
{
}

std::string LogMessage::format() const
{
	return fmt::format("[{}] [{}] {}", getTime(), logTypeString(), message);
}

std::string LogMessage::logTypeString() const
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

std::string LogMessage::getTime() const
{
	char buffer[64];
	std::strftime(buffer, sizeof(buffer), "%X", std::localtime(&time));
	return std::string(buffer);
}

std::string LogMessage::getType() const
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

std::string LogMessage::getMessage() const
{
	return message;
}
