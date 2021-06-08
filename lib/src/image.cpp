#include "lib/image.hpp"

auto lib::image::is_jpeg(const std::vector<unsigned char> &data) -> bool
{
	if (data.size() < 3)
	{
		return false;
	}

	// JPEG magic number
	return data.at(0) == 0xff
		&& data.at(1) == 0xd8
		&& data.at(2) == 0xff;
}
