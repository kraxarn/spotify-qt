#include "lib/datetime.hpp"
#include "thirdparty/doctest.h"

TEST_CASE("date_time::now(_utc)")
{
	lib::date_time date_time;

	SUBCASE("now")
	{
		date_time = lib::date_time::now();

		// We can't really make sure it's valid, but it should be after 2021-01-01
		CHECK(date_time.is_valid());
		CHECK(date_time.get_year() >= 2021);
		CHECK(date_time.get_month() >= 1);
		CHECK(date_time.get_day() >= 1);
	}

	SUBCASE("now utc")
	{
		date_time = lib::date_time::now_utc();

		// Similar issue like with now
		CHECK(date_time.is_valid());
		CHECK(date_time.get_year() >= 2021);
		CHECK(date_time.get_month() >= 1);
		CHECK(date_time.get_day() >= 1);
	}
}

TEST_CASE("date_time::parse")
{
	lib::date_time date_time;

	auto validate_date = [&date_time](int year, int month, int day,
		int hour = 0, int minute = 0, int second = 0)
	{
			CHECK(date_time.is_valid());

			CHECK_EQ(year, date_time.get_year());
			CHECK_EQ(month, date_time.get_month());
			CHECK_EQ(day, date_time.get_day());
			CHECK_EQ(hour, date_time.get_hour());
			CHECK_EQ(minute, date_time.get_minute());
			CHECK_EQ(second, date_time.get_second());
	};

	SUBCASE("iso date time")
	{
		date_time = lib::date_time::parse("2001-02-03T14:15:16Z");
		validate_date(2001, 2, 3, 14, 15, 16);
	}

	SUBCASE("iso date")
	{
		date_time = lib::date_time::parse("2002-03-04");
		validate_date(2002, 3, 4);
	}
}

TEST_CASE("date_time::to_iso_date(_time)")
{
	lib::date_time date_time;

	SUBCASE("iso date")
	{
		date_time = lib::date_time(2005, 6, 7, 0, 0, 0);
		CHECK_EQ(date_time.to_iso_date(), "2005-06-07");
	}

	SUBCASE("iso date time")
	{
		date_time = lib::date_time(2008, 9, 10, 11, 12, 14);
		CHECK_EQ(date_time.to_iso_date_time(), "2008-09-10T11:12:14Z");
	}
}