#include "lib/datetime.hpp"
#include "lib/log.hpp"

#define LOCALE_TIME_FORMAT "%X"
#define LOCALE_DATE_FORMAT "%x"
#define ISO_DATE_FORMAT "%Y-%m-%d"
#define ISO_DATE_TIME_FORMAT "%Y-%m-%dT%H:%M:%SZ"

lib::date_time::date_time(const date_time &date)
{
	tm = date.tm;
}

lib::date_time::date_time(int year, int month, int day, int hour, int minute, int second)
{
	tm.tm_year = year - c_year_offset;
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = second;
}

auto lib::date_time::parse(const std::string &value) -> lib::date_time
{
	lib::date_time date;

	// First try to parse as full date and time
	date.parse(value, ISO_DATE_TIME_FORMAT);

	// Then try to parse as date only
	if (!date.is_valid())
	{
		date.parse(value, ISO_DATE_FORMAT);
	}

	// If it's still invalid, something is wrong
	if (!date.is_valid())
	{
		log::warn("Failed to parse \"{}\" as a date", value);
	}

	return date;
}

auto lib::date_time::now() -> lib::date_time
{
	lib::date_time date;
	auto time = std::time(nullptr);
	date.tm = *std::localtime(&time);

	return date;
}

auto lib::date_time::now_utc() -> lib::date_time
{
	lib::date_time date;
	auto time = std::time(nullptr);
	date.tm = *std::gmtime(&time);

	return date;
}

auto lib::date_time::seconds_since_epoch() -> long
{
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now()
		.time_since_epoch()).count();
}

auto lib::date_time::is_valid() const -> bool
{
	return tm.tm_year > 0
		|| tm.tm_mon > 0
		|| tm.tm_mday > 0;
}

void lib::date_time::parse(const std::string &value, const char *format)
{
	std::istringstream ss(value);
	ss >> std::get_time(&tm, format);
}

auto lib::date_time::to_time() const -> std::string
{
	return format(LOCALE_TIME_FORMAT);
}

auto lib::date_time::to_date() const -> std::string
{
	return format(LOCALE_DATE_FORMAT);
}

auto lib::date_time::to_iso_date() const -> std::string
{
	return format(ISO_DATE_FORMAT);
}

auto lib::date_time::to_iso_date_time() const -> std::string
{
	return format(ISO_DATE_TIME_FORMAT);
}

auto lib::date_time::format(const char *format) const -> std::string
{
	if (!is_valid())
	{
		return std::string();
	}

	constexpr size_t buffer_size = 64;
	std::array<char, buffer_size> buffer;
	std::strftime(buffer.data(), buffer.size(), format, &tm);
	return std::string(buffer.data());
}

auto lib::date_time::get_second() const -> int
{
	return is_valid()
		? tm.tm_sec
		: 0;
}

auto lib::date_time::get_minute() const -> int
{
	return is_valid()
		? tm.tm_min
		: 0;
}

auto lib::date_time::get_hour() const -> int
{
	return is_valid()
		? tm.tm_hour
		: 0;
}

auto lib::date_time::get_day() const -> int
{
	return is_valid()
		? tm.tm_mday
		: 0;
}

auto lib::date_time::get_month() const -> int
{
	return 1 + (is_valid()
		? tm.tm_mon
		: 0);
}

auto lib::date_time::get_year() const -> int
{
	return c_year_offset + (is_valid()
		? tm.tm_year
		: 0);
}
