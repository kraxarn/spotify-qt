#include "log.hpp"

void Log::log(LogType logType, const QString &message)
{
	auto msg = QString("[%1] [%2] %3")
		.arg(QLocale::system().toString(QDateTime::currentDateTime().time(), QLocale::ShortFormat))
		.arg(logTypeString(logType))
		.arg(message)
		.toStdString();

	if (logType == LogType::Information)
		std::cout << msg << std::endl;
	else
		std::cerr << msg << std::endl;
}

void Log::info(const QString &message)
{
	log(LogType::Information, message);
}

void Log::warn(const QString &message)
{
	log(LogType::Warning, message);
}

void Log::error(const QString &message)
{
	log(LogType::Error, message);
}

QString Log::logTypeString(LogType logType)
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

	return QString();
}
