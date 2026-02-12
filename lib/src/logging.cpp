#include "lib/logging.hpp"
#include "lib/log.hpp"

#include <QtMessageHandler>

QtMessageHandler Logging::mDefaultHandler = nullptr;

QList<LogMessage> Logging::mMessages = QList<LogMessage>();
bool Logging::mLogToStdout = true;

void Logging::installMessageHandler()
{
	qSetMessagePattern(QStringLiteral(
		"[%{time hh:mm:ss}] ["
		"%{if-debug}dbg%{endif}"
		"%{if-info}inf%{endif}"
		"%{if-warning}wrn%{endif}"
		"%{if-critical}err%{endif}"
		"%{if-fatal}ftl%{endif}"
		"] %{message}"
	));

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

void Logging::message(const QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	const LogMessage message(type, msg);
	mMessages.push_back(message);

	mDefaultHandler(type, context, msg);
}
