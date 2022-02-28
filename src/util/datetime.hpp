#pragma once

#include <QString>
#include <QDateTime>

class DateTime
{
public:
	/**
	 * Format date as "... ago"
	 */
	static auto toRelative(const QDateTime &date) -> QString;

	/**
	 * Format ISO date as "... ago"
	 */
	static auto toRelative(const std::string &date) -> QString;

	/**
	 * Is date empty (1970-01-01)
	 */
	static auto isEmpty(const QDateTime &date) -> bool;

	/**
	 * Parse ISO date
	 */
	static auto parseIso(const QString &date) -> QDateTime;

	/**
	 * Parse ISO date
	 */
	static auto parseIso(const std::string &date) -> QDateTime;

	/**
	 * Parse date as YYYY-MM-DD, YYYY-MM, or YYYY
	 */
	static auto parseIsoDate(const QString &date) -> QDateTime;

	/**
	 * Parse date as YYYY-MM-DD, YYYY-MM, or YYYY
	 */
	static auto parseIsoDate(const std::string &date) -> QDateTime;

private:
	DateTime() = default;

	static constexpr unsigned int secsInMinute = 60;
	static constexpr unsigned int secsInHour = secsInMinute * 60;
	static constexpr unsigned int secsInDay = secsInHour * 24;
	static constexpr unsigned int secsInWeek = secsInDay * 7;
	static constexpr unsigned int secsInMonth = secsInWeek * 4.35;
	static constexpr unsigned int secsInYear = secsInMonth * 12;
};
