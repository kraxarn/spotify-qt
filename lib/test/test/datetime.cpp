#include <gtest/gtest.h>

#include "lib/datetime.hpp"

class date_time_tests: public testing::Test
{
protected:
	lib::date_time date_time;

	void SetUp() override
	{
		// ...
	}

	void validate_date(int year, int month, int day,
		int hour = 0, int minute = 0, int second = 0)
	{
		ASSERT_TRUE(date_time.is_valid());

		ASSERT_EQ(year, date_time.get_year());
		ASSERT_EQ(month, date_time.get_month());
		ASSERT_EQ(day, date_time.get_day());
		ASSERT_EQ(hour, date_time.get_hour());
		ASSERT_EQ(minute, date_time.get_minute());
		ASSERT_EQ(second, date_time.get_second());
	}
};

TEST_F(date_time_tests, can_get_now)
{
	date_time = lib::date_time::now();

	// We can't really make sure it's valid, but it should be after 2021-01-01
	ASSERT_TRUE(date_time.is_valid());
	ASSERT_TRUE(date_time.get_year() >= 2021);
	ASSERT_TRUE(date_time.get_month() >= 1);
	ASSERT_TRUE(date_time.get_day() >= 1);
}

TEST_F(date_time_tests, can_get_now_utc)
{
	date_time = lib::date_time::now_utc();

	// Similar issue like with now
	ASSERT_TRUE(date_time.is_valid());
	ASSERT_TRUE(date_time.get_year() >= 2021);
	ASSERT_TRUE(date_time.get_month() >= 1);
	ASSERT_TRUE(date_time.get_day() >= 1);
}

TEST_F(date_time_tests, can_parse_iso_date_time)
{
	date_time = lib::date_time::parse("2001-02-03T14:15:16Z");
	validate_date(2001, 02, 03, 14, 15, 16);
}

TEST_F(date_time_tests, can_parse_iso_date)
{
	date_time = lib::date_time::parse("2002-03-04");
	validate_date(2002, 03, 04);
}
