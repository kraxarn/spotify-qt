#include "lib/logmessage.hpp"

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
