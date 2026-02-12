#pragma once

#include <QDateTime>
#include <QtLogging>

class LogMessage
{
public:
	LogMessage();

	LogMessage(QtMsgType type, const QString &message);

	[[nodiscard]]
	auto dateTime() const -> const QDateTime &;

	[[nodiscard]]
	auto type() const -> QtMsgType;

	[[nodiscard]]
	auto message() const -> const QString &;

	[[nodiscard]]
	auto timeString() const -> QString;

	[[nodiscard]]
	auto typeShortString() const -> QString;

	[[nodiscard]]
	auto typeLongString() const -> QString;

	[[nodiscard]]
	auto toString() const -> QString;

private:
	QDateTime mDateTime;
	QtMsgType mType;
	QString mMessage;
};
