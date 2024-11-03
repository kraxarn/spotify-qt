#include "util/datetime.hpp"

auto DateTime::toRelative(const QDateTime &date) -> QString
{
	auto amount = date.secsTo(QDateTime::currentDateTimeUtc());
	if (amount <= 0)
	{
		return QStringLiteral("Just now");
	}

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

	return QStringLiteral("%1 %2%3 ago")
		.arg(amount)
		.arg(unit, amount == 1 ? "" : "s");
}

auto DateTime::toRelative(const std::string &date) -> QString
{
	const auto parsed = parseIsoDate(date);

	return parsed.isValid()
		? toRelative(parsed)
		: QString();
}

auto DateTime::isEmpty(const QDateTime &date) -> bool
{
	return date.toSecsSinceEpoch() <= 0;
}

auto DateTime::parseIso(const QString &date) -> QDateTime
{
	return QDateTime::fromString(date, Qt::ISODate);
}

auto DateTime::parseIso(const std::string &date) -> QDateTime
{
	return parseIso(QString::fromStdString(date));
}

auto DateTime::parseIsoDate(const QString &date) -> QDateTime
{
	return parseIso(date
		+ QStringLiteral("-01").repeated(2 - date.count('-')));
}

auto DateTime::parseIsoDate(const std::string &date) -> QDateTime
{
	return parseIsoDate(QString::fromStdString(date));
}

auto DateTime::epoch() -> QDateTime
{
	const QDate date(1970, 1, 1);
	const QTime time(0, 0);
	return {date, time};
}
