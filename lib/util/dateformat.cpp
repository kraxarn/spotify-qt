#include "dateformat.hpp"

using namespace lib;

#define LOCAL_TIME_FORMAT "%X"
#define LOCALE_DATE_FORMAT "%x"
#define ISO_DATE_FORMAT "%Y-%m-%d"
#define ISO_DATE_TIME_FORMAT "%Y-%m-%dT%H:%M:%SZ"

std::string date::format(Format format, std::tm *date)
{
	if (date == nullptr)
		return std::string();

	char buffer[64];
	std::strftime(buffer, sizeof(buffer), formatString(format), date);
	return std::string(buffer);
}

void date::parse(const std::string &value, Format format, std::tm *date)
{
	if (date != nullptr)
		std::stringstream(value) >> std::get_time(date, formatString(format));
}

const char *date::formatString(Format format)
{
	switch (format)
	{
		case Format::LocalTime:
			return LOCAL_TIME_FORMAT;

		case Format::LocalDate:
			return LOCALE_DATE_FORMAT;

		case Format::IsoDate:
			return ISO_DATE_FORMAT;

		case Format::IsoDateTime:
			return ISO_DATE_TIME_FORMAT;
	}
}
