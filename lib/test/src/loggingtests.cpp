#include "thirdparty/doctest.h"
#include "lib/logging.hpp"
#include "lib/developermode.hpp"

#include <QtLogging>

TEST_CASE("Logging")
{
	auto initLog = []() -> void
	{
		Logging::clear();
		Logging::setLogToStdout(false);
		Logging::installMessageHandler();
	};

	auto verifyMessages = []() -> void
	{
		const QList<LogMessage> &messages = Logging::messages();
		CHECK_EQ(messages.length(), 2);

		for (auto &message: messages)
		{
			CHECK_EQ(message.message(), "hello world");
		}
	};

	SUBCASE("info")
	{
		initLog();
		qInfo() << "hello world";
		qInfo() << "hello" << "world";
		verifyMessages();
	}

	SUBCASE("warn")
	{
		initLog();
		qWarning() << "hello world";
		qWarning() << "hello" << "world";
		verifyMessages();
	}

	SUBCASE("error")
	{
		initLog();
		qCritical() << "hello world";
		qCritical() << "hello" << "world";
		verifyMessages();
	}

	SUBCASE("debug")
	{
		initLog();

		qDebug() << "hello world";
		qDebug() << "hello" "world";
		CHECK_EQ(Logging::messages().length(), 0);

		lib::developer_mode::enabled = true;
		qDebug() << "hello world";
		qDebug() << "hello" << "world";
		verifyMessages();
	}
}
