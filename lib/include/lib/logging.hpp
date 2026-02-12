#pragma once

#include "lib/logmessage.hpp"

#include <QString>
#include <QtMessageHandler>

namespace lib
{
	class log;
}

class Logging
{
public:
	static void installMessageHandler();

	static void setLogToStdout(bool value);

	[[nodiscard]]
	static auto messages() -> const QList<LogMessage> &;

	static void clear();

private:
	Logging() = default;

	static QtMessageHandler mDefaultHandler;

	static QList<LogMessage> mMessages;
	static bool mLogToStdout;

	static void message(QtMsgType type, const QMessageLogContext &context, const QString &msg);

	friend class lib::log;
};
