#include "thirdparty/doctest.h"
#include "lib/image.hpp"

TEST_CASE("image")
{
	SUBCASE("is_jpeg")
	{
		// Empty data
		std::vector<unsigned char> data;
		CHECK_FALSE(lib::image::is_jpeg(data));

		// Invalid data (PNG)
		data = {
			// PNG magic number
			0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a
		};
		CHECK_FALSE(lib::image::is_jpeg(data));

		// Valid data
		data = {
			// JPEG magic number
			0xff, 0xd8, 0xff
		};
		CHECK(lib::image::is_jpeg(data));
	}
}
