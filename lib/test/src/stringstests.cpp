#include "lib/strings.hpp"
#include "thirdparty/doctest.h"

#include <array>

TEST_CASE("strings")
{
	SUBCASE("join")
	{
		auto result = lib::strings::join({
			"a", "bb", "ccc",
		}, ", ");

		CHECK_EQ(result, "a, bb, ccc");
	}

	SUBCASE("contains")
	{
		CHECK(lib::strings::contains("aa,bb,cc", "aa"));
		CHECK_FALSE(lib::strings::contains("aa,bb,cc", "dd"));
	}

	SUBCASE("trim")
	{
		auto verify_trimmed = [](std::string &str, const char *expected = "aa")
		{
			CHECK_EQ(lib::strings::trim(str), "aa");
			CHECK_EQ(str, "aa");
		};

		// Left
		std::string str("  aa");
		verify_trimmed(str);

		// Right
		str = "aa  ";
		verify_trimmed(str);

		// Left and right
		str = "  aa  ";
		verify_trimmed(str);
	}

	SUBCASE("split")
	{
		// With delimiter
		auto expected = std::array<std::string, 3>{
			"a", "bb", "ccc",
		};
		auto results = lib::strings::split("a,bb,ccc", ',');

		CHECK_EQ(results.size(), expected.size());
		for (size_t i = 0; i < results.size(); i++)
		{
			CHECK_EQ(results[i], expected[i]);
		}

		// Without delimiter
		results = lib::strings::split("a,bb,ccc", '.');
		CHECK_EQ(results.size(), 1);
		CHECK_EQ(results[0], "a,bb,ccc");

		// Empty string
		results = lib::strings::split(std::string(), ',');
		CHECK_EQ(results.size(), 1);
		CHECK(results[0].empty());

		// String delimiter
		expected = std::array<std::string, 3>{
			"a", "bb", "ccc",
		};
		results = lib::strings::split("a, bb, ccc", ", ");

		CHECK_EQ(results.size(), expected.size());
		for (size_t i = 0; i < results.size(); i++)
		{
			CHECK_EQ(results[i], expected[i]);
		}

		// Without string delimiter
		CHECK_EQ(lib::strings::split("a, bb, ccc", ". ").at(0), "a, bb, ccc");

		// Empty string delimiter
		CHECK(lib::strings::split(std::string(), ";").at(0).empty());
	}

	SUBCASE("starts_with")
	{
		CHECK(lib::strings::starts_with("aa,bb,cc", "aa"));
		CHECK_FALSE(lib::strings::starts_with("aa,bb,cc", "ab"));
	}

	SUBCASE("remove")
	{
		auto verify_removed = [](std::string &str,
			const std::string &substr, const std::string &expected)
		{
			CHECK_EQ(lib::strings::remove(str, substr), expected);
			CHECK_EQ(str, expected);
		};

		// Start
		std::string str("aa,bb,cc");
		verify_removed(str, "aa", ",bb,cc");

		// Middle
		str = "aa,bb,cc";
		verify_removed(str, "bb", "aa,,cc");

		// End
		str = "aa,bb,cc";
		verify_removed(str, "cc", "aa,bb,");
	}

	SUBCASE("ends_with")
	{
		CHECK(lib::strings::ends_with("aa,bb,cc", "cc"));
		CHECK_FALSE(lib::strings::ends_with("aa,bb,cc", "bc"));
	}

	SUBCASE("try_to_int")
	{
		// Valid
		auto i = 0;
		CHECK(lib::strings::try_to_int("10", i));
		CHECK_EQ(i, 10);

		// Invalid
		i = 0;
		CHECK_FALSE(lib::strings::try_to_int("a", i));
		CHECK_EQ(i, 0);

		// Overflow
		i = 0;
		const auto int_max = static_cast<unsigned int>(std::numeric_limits<int>::max());
		CHECK_THROWS(lib::strings::try_to_int(std::to_string(int_max + 1), i));
		CHECK_EQ(i, 0);
	}

	SUBCASE("left")
	{
		CHECK_EQ(lib::strings::left("aa,bb,cc", 2), "aa");
		CHECK_EQ(lib::strings::left("aa", 5), "aa");
	}

	SUBCASE("right")
	{
		CHECK_EQ(lib::strings::right("aa,bb,cc", 2), "cc");
		CHECK_EQ(lib::strings::right("aa", 5), "aa");
	}

	SUBCASE("to_lower")
	{
		std::string str("aa,BB,Cc");
		CHECK_EQ(lib::strings::to_lower(str), "aa,bb,cc");
	}

	SUBCASE("to_upper")
	{
		std::string str("aa,BB,Cc");
		CHECK_EQ(lib::strings::to_upper(str), "AA,BB,CC");
	}

	SUBCASE("capitalize")
	{
		CHECK_EQ(lib::strings::capitalize("Hello World"), "Hello world");
		CHECK_EQ(lib::strings::capitalize("HELLO WORLD"), "Hello world");
		CHECK_EQ(lib::strings::capitalize("hello world"), "Hello world");

		CHECK_EQ(lib::strings::capitalize(""), "");
		CHECK_EQ(lib::strings::capitalize("h"), "H");
	}

	SUBCASE("to_string")
	{
		constexpr double pi = 3.14159265;

		CHECK_EQ(lib::strings::to_string(pi, 0), "3");
		CHECK_EQ(lib::strings::to_string(pi, 3), "3.142");
	}

	SUBCASE("replace_all")
	{
		// Char
		std::string val("a,b,c");
		CHECK_EQ(lib::strings::replace_all(val, 'c', 'd'), "a,b,d");
		CHECK_EQ(lib::strings::replace_all(val, ',', ' '), "a b c");
		CHECK_EQ(lib::strings::replace_all(val, 'd', 'c'), "a,b,c");

		// String
		val = "abc, def, ghi";
		CHECK_EQ(lib::strings::replace_all(val, "ghi", "jkl"), "abc, def, jkl");
		CHECK_EQ(lib::strings::replace_all(val, ", ", "->"), "abc->def->ghi");
		CHECK_EQ(lib::strings::replace_all(val, "adg", "mno"), "abc, def, ghi");
	}

	SUBCASE("erase_non_alpha")
	{
		CHECK_EQ(lib::strings::erase_non_alpha("Hello, World"), "HelloWorld");
		CHECK_EQ(lib::strings::erase_non_alpha("He110 (W047d)"), "HeWd");
		CHECK_EQ(lib::strings::erase_non_alpha("hello"), "hello");
	}
}
