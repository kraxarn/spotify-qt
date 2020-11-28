#pragma once

#include "enum/logtype.hpp"
#include "format.hpp"

#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace lib
{
	class LogMessage
	{
	public:
		LogMessage(const std::time_t &time, LogType logType, const std::string &message);
		LogMessage(LogType logType, const std::string &message);

		std::string format() const;
		std::string logTypeString() const;

		std::string getTime() const;
		std::string getType() const;
		std::string getMessage() const;

	private:
		std::time_t time;
		LogType logType;
		std::string message;
	};
}