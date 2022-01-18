#include "lib/uri.hpp"
#include "thirdparty/doctest.h"

TEST_CASE("uri")
{
	SUBCASE("validate")
	{
		CHECK_NOTHROW(lib::uri("https://example.com"));
		CHECK_NOTHROW(lib::uri("https://sub.example.com"));
		CHECK_NOTHROW(lib::uri("https://example.com/page?key=value"));

		CHECK_THROWS(lib::uri("http:example.com"));
		CHECK_THROWS(lib::uri("example.com"));
		CHECK_THROWS(lib::uri("example"));
	}

	SUBCASE("hostname")
	{
		lib::uri uri("https://example.com");
		CHECK_EQ(uri.hostname(), "example.com");

		uri.set_url("https://example.com/page");
		CHECK_EQ(uri.hostname(), "example.com");

		uri.set_url("https://sub.example.com/page");
		CHECK_EQ(uri.hostname(), "sub.example.com");
	}

	SUBCASE("origin")
	{
		lib::uri uri("https://example.com");
		CHECK_EQ(uri.origin(), "https://example.com");

		uri.set_url("https://example.com/page");
		CHECK_EQ(uri.origin(), "https://example.com");
	}

	SUBCASE("pathname")
	{
		lib::uri uri("https://example.com");
		CHECK_EQ(uri.pathname(), "");

		uri.set_url("https://example.com/page");
		CHECK_EQ(uri.pathname(), "/page");

		uri.set_url("https://example.com/page/index");
		CHECK_EQ(uri.pathname(), "/page/index");
	}

	SUBCASE("protocol")
	{
		lib::uri uri("https://example.com");
		CHECK_EQ(uri.protocol(), "https:");

		uri.set_url("http://example.com");
		CHECK_EQ(uri.protocol(), "http:");
	}

	SUBCASE("get_search_params")
	{
		lib::uri uri("https://example.com");
		auto params = uri.get_search_params();
		CHECK(params.empty());

		uri.set_url("https://example.com?key1=value1&key2=value2");
		params = uri.get_search_params();

		CHECK_FALSE(params.empty());
		CHECK_EQ(params.at("key1"), "value1");
		CHECK_EQ(params.at("key2"), "value2");
	}

	SUBCASE("set_search_params")
	{
		lib::uri uri("https://example.com");

		std::map<std::string, std::string> params{
			{"key1", "value1"},
			{"key2", "value2"},
		};
		uri.set_search_params(params);

		const std::string expected("https://example.com?key1=value1&key2=value2");
		CHECK_EQ(uri.get_url(), expected);

		uri.set_url("https://example.com?old_key=old_value");
		uri.set_search_params(params);
		CHECK_EQ(uri.get_url(), expected);

		uri.set_url("https://example.com?key=value");
		uri.set_search_params({});
		CHECK_EQ(uri.get_url(), "https://example.com");
	}

	SUBCASE("encode")
	{
		CHECK_EQ(lib::uri::encode("/Hello World/"), "%2FHello%20World%2F");
	}
}
