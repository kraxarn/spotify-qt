#pragma once

#include <QString>
#include <QtMessageHandler>

class Logging
{
public:
	static void installMessageHandler();

private:
	Logging() = default;

	static QtMessageHandler mDefaultHandler;

	static void message(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};
