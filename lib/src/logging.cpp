#include "lib/logging.hpp"
#include "lib/log.hpp"

#include <iostream>

#include <QtMessageHandler>

QtMessageHandler Logging::mDefaultHandler = nullptr;

QList<LogMessage> Logging::mMessages = QList<LogMessage>();
bool Logging::mLogToStdout = true;

void Logging::installMessageHandler()
{
	mDefaultHandler = qInstallMessageHandler(message);
}

void Logging::setLogToStdout(const bool value)
{
	mLogToStdout = value;
}

auto Logging::messages() -> const QList<LogMessage> &
{
	return mMessages;
}

void Logging::clear()
{
	mMessages.clear();
}

void Logging::message(const QtMsgType type, [[maybe_unused]] const QMessageLogContext &context, const QString &msg)
{
	const LogMessage message(type, msg);
	mMessages.push_back(message);

	// TODO: Do we want qSetMessagePattern instead?
	(type == QtInfoMsg || type == QtDebugMsg ? std::cout : std::cerr)
		<< message.toString().toStdString() << '\n';
}
