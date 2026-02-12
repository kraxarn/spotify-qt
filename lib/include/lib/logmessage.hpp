#pragma once

#include "lib/datetime.hpp"
#include "lib/enum/logtype.hpp"

#include <QDateTime>
#include <QtLogging>

class LogMessage
{
public:
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
	LogMessage();

	QDateTime mDateTime;
	QtMsgType mType;
	QString mMessage;
};

namespace lib
{
	/**
	 * A message in the log
	 */
	class [[deprecated]] log_message
	{
	public:
		/**
		 * Save a logged message for the specified time
		 * @param time Time it was logged
		 * @param log_type Type of log
		 * @param message Message logged
		 */
		log_message(const date_time &time, log_type log_type,
			const std::string &message);

		/**
		 * Save a logged message for the current time
		 * @param log_type Type of log
		 * @param message Message logged
		 */
		log_message(log_type log_type, const std::string &message);

		/**
		 * Instance with default values, only use if required
		 */
		log_message() = default;

		/**
		 * Format message as [time] [type] message
		 * @return Formatted message
		 */
		auto to_string() const -> std::string;

		/**
		 * log_type as 4 character string
		 * @return Type of log
		 */
		auto get_type_short() const -> std::string;

		/**
		 * Get the time the message was logged at
		 * @return Time as locale formatted string
		 */
		auto get_time() const -> std::string;

		/**
		 * log_type as a full string
		 * @return Type of log
		 */
		auto get_type() const -> std::string;

		/**
		 * Get logged message
		 * @return Message
		 */
		auto get_message() const -> std::string;

	private:
		/**
		 * Logged time
		 */
		date_time time;

		/**
		 * Type of log
		 */
		log_type logType;

		/**
		 * Message logged
		 */
		std::string message;
	};
}
