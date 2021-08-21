#include "dateutils.hpp"

auto DateUtils::toRelative(const QDateTime &date) -> QString
{
	auto amount = date.secsTo(QDateTime::currentDateTimeUtc());
	QString unit = "second";

	if (amount >= secsInYear)
	{
		amount /= secsInYear;
		unit = "year";
	}
	else if (amount >= secsInMonth)
	{
		amount /= secsInMonth;
		unit = "month";
	}
	else if (amount >= secsInWeek)
	{
		amount /= secsInWeek;
		unit = "week";
	}
	else if (amount >= secsInDay)
	{
		amount /= secsInDay;
		unit = "day";
	}
	else if (amount >= secsInHour)
	{
		amount /= secsInHour;
		unit = "hour";
	}
	else if (amount >= secsInMinute)
	{
		amount /= secsInMinute;
		unit = "minute";
	}

	return QString("%1 %2%3 ago")
		.arg(amount)
		.arg(unit, amount == 1 ? "" : "s");
}

auto DateUtils::toRelative(const std::string &date) -> QString
{
	return toRelative(fromIso(date));
}

auto DateUtils::isEmpty(const QDateTime &date) -> bool
{
	return date.toSecsSinceEpoch() <= 0;
}

auto DateUtils::fromIso(const QString &date) -> QDateTime
{
	return QDateTime::fromString(date, Qt::ISODate);
}

auto DateUtils::fromIso(const std::string &date) -> QDateTime
{
	return fromIso(QString::fromStdString(date));
}
