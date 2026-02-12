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
			return "inf";

		case log_type::warning:
			return "wrn";

		case log_type::error:
			return "err";

		case log_type::verbose:
			return "dbg";
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

LogMessage::LogMessage(const QtMsgType type, const QString &message)
	: mDateTime(QDateTime::currentDateTime()),
	mType(type),
	mMessage(message)
{
}

LogMessage::LogMessage()
	: mType(QtInfoMsg)
{
}

auto LogMessage::dateTime() const -> const QDateTime &
{
	return mDateTime;
}

auto LogMessage::type() const -> QtMsgType
{
	return mType;
}

auto LogMessage::message() const -> const QString &
{
	return mMessage;
}

auto LogMessage::timeString() const -> QString
{
	return QLocale::system().toString(dateTime().time());
}

auto LogMessage::typeShortString() const -> QString
{
	switch (mType)
	{
		case QtDebugMsg:
			return QStringLiteral("dbg");

		case QtWarningMsg:
			return QStringLiteral("wrn");

		case QtCriticalMsg:
			return QStringLiteral("err");

		case QtFatalMsg:
			return QStringLiteral("ftl");

		case QtInfoMsg:
			return QStringLiteral("inf");

		default:
			return QString();
	}
}

auto LogMessage::typeLongString() const -> QString
{
	switch (mType)
	{
		case QtDebugMsg:
			return QStringLiteral("Debug");

		case QtWarningMsg:
			return QStringLiteral("Warning");

		case QtCriticalMsg:
			return QStringLiteral("Error");

		case QtFatalMsg:
			return QStringLiteral("Fatal");

		case QtInfoMsg:
			return QStringLiteral("Info");

		default:
			return QString();
	}
}

auto LogMessage::toString() const -> QString
{
	return QStringLiteral("[%1] [%2] %3")
		.arg(timeString(), typeShortString(), message());
}
