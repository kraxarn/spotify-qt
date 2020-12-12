#include "date.hpp"

using namespace lib;

#define LOCAL_TIME_FORMAT "%X"
#define LOCALE_DATE_FORMAT "%x"
#define ISO_DATE_FORMAT "%Y-%m-%d"
#define ISO_DATE_TIME_FORMAT "%Y-%m-%dT%H:%M:%SZ"

date date::parse(const std::string &value)
{
	lib::date date;

	// First try to parse as full date and time
	date.parse(value, ISO_DATE_TIME_FORMAT);

	// Then try to pars as date only
	if (!date.is_valid())
		date.parse(value, ISO_DATE_FORMAT);

	// If it's still invalid, something is wrong
	if (!date.is_valid())
		log::warn("Failed to parse \"{}\" as a date")
}

date date::now()
{
	date date;
	auto time = std::time(nullptr);
	date.tm = std::localtime(&time);

	return date;
}

date date::now_utc()
{
	date date;
	auto time = std::time(nullptr);
	date.tm = std::gmtime(&time);

	return date;
}

bool date::is_valid() const
{
	return tm != nullptr;
}

void date::parse(const std::string &value, const char *format)
{
	std::stringstream(value) >> std::get_time(tm, format);
}

std::string date::to_time() const
{
	return format(LOCAL_TIME_FORMAT);
}

std::string date::to_date() const
{
	return format(LOCALE_DATE_FORMAT);
}

std::string date::to_iso_date() const
{
	return format(ISO_DATE_FORMAT);
}

std::string date::to_iso_date_time() const
{
	return format(ISO_DATE_TIME_FORMAT);
}

std::string date::format(const char *format) const
{
	if (tm == nullptr)
		return std::string();

	char buffer[64];
	std::strftime(buffer, sizeof(buffer), format, tm);
	return std::string(buffer);
}

bool date::is_empty() const
{
	return tm == nullptr
		|| (tm->tm_year <= 70
			&& tm->tm_mon == 0
			&& tm->tm_mday == 0);
}
