#pragma once

#include <QString>
#include <QDateTime>

class DateUtils
{
public:
	static auto toRelative(const QDateTime &date) -> QString;
	static auto toRelative(const std::string &date) -> QString;
	static auto isEmpty(const QDateTime &date) -> bool;
	static auto fromIso(const QString &date) -> QDateTime;
	static auto fromIso(const std::string &date) -> QDateTime;

private:
	DateUtils() = default;

	static constexpr unsigned int secsInMinute = 60;
	static constexpr unsigned int secsInHour = secsInMinute * 60;
	static constexpr unsigned int secsInDay = secsInHour * 24;
	static constexpr unsigned int secsInWeek = secsInDay * 7;
	static constexpr unsigned int secsInMonth = secsInWeek * 4.35;
	static constexpr unsigned int secsInYear = secsInMonth * 12;
};
