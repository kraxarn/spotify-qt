#include "lib/logging.hpp"
#include "lib/log.hpp"

#include <iostream>

#include <QtMessageHandler>

QtMessageHandler Logging::mDefaultHandler = nullptr;

void Logging::installMessageHandler()
{
	mDefaultHandler = qInstallMessageHandler(message);
}

void Logging::message(const QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	lib::log_type logType;
	switch (type)
	{
		case QtWarningMsg:
			logType = lib::log_type::warning;
			break;

		case QtCriticalMsg:
		case QtFatalMsg:
			logType = lib::log_type::error;
			break;

		case QtInfoMsg:
			logType = lib::log_type::information;
			break;

		default:
			logType = lib::log_type::verbose;
			break;
	}

	const lib::log_message message(logType, msg.toStdString());
	lib::log::messages.push_back(message);

	(type == QtInfoMsg || type == QtDebugMsg ? std::cout : std::cerr)
		<< message.to_string() << std::endl;
}
