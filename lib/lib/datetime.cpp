#include "datetime.hpp"
#include "log/log.hpp"

using namespace lib;

#define LOCALE_TIME_FORMAT "%X"
#define LOCALE_DATE_FORMAT "%x"
#define ISO_DATE_FORMAT "%Y-%m-%d"
#define ISO_DATE_TIME_FORMAT "%Y-%m-%dT%H:%M:%SZ"

date_time::date_time(const date_time &date)
{
	tm = date.tm;
}

date_time::date_time(int year, int month, int day, int hour, int minute, int second)
{
	tm.tm_year = year - 1900;
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = second;
}

date_time date_time::parse(const std::string &value)
{
	date_time date;

	// First try to parse as full date and time
	date.parse(value, ISO_DATE_TIME_FORMAT);

	// Then try to pars as date only
	if (!date.is_valid())
		date.parse(value, ISO_DATE_FORMAT);

	// If it's still invalid, something is wrong
	if (!date.is_valid())
		log::warn("Failed to parse \"{}\" as a date", value);

	return date;
}

date_time date_time::now()
{
	date_time date;
	auto time = std::time(nullptr);
	date.tm = *std::localtime(&time);

	return date;
}

date_time date_time::now_utc()
{
	date_time date;
	auto time = std::time(nullptr);
	date.tm = *std::gmtime(&time);

	return date;
}

bool date_time::is_valid() const
{
	return tm.tm_year > 0
		|| tm.tm_mon > 0
		|| tm.tm_mday > 0;
}

void date_time::parse(const std::string &value, const char *format)
{
	std::istringstream ss(value);
	ss >> std::get_time(&tm, format);
}

std::string date_time::to_time() const
{
	return format(LOCALE_TIME_FORMAT);
}

std::string date_time::to_date() const
{
	return format(LOCALE_DATE_FORMAT);
}

std::string date_time::to_iso_date() const
{
	return format(ISO_DATE_FORMAT);
}

std::string date_time::to_iso_date_time() const
{
	return format(ISO_DATE_TIME_FORMAT);
}

std::string date_time::format(const char *format) const
{
	if (!is_valid())
		return std::string();

	char buffer[64];
	std::strftime(buffer, sizeof(buffer), format, &tm);
	return std::string(buffer);
}

int date_time::get_second() const
{
	return is_valid()
		? tm.tm_sec
		: 0;
}

int date_time::get_minute() const
{
	return is_valid()
		? tm.tm_min
		: 0;
}

int date_time::get_hour() const
{
	return is_valid()
		? tm.tm_hour
		: 0;
}

int date_time::get_day() const
{
	return is_valid()
		? tm.tm_mday
		: 0;
}

int date_time::get_month() const
{
	return 1 + (is_valid()
		? tm.tm_mon
		: 0);
}

int date_time::get_year() const
{
	return 1900 + (is_valid()
		? tm.tm_year
		: 0);
}
