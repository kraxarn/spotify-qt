#pragma once

#include <vector>

namespace lib
{
	/**
	 * Image utilities
	 */
	class image
	{
	public:
		/**
		 * Check if data is a valid jpeg image
		 * @param data Image data
		 */
		static auto is_jpeg(const std::vector<unsigned char> &data) -> bool;

	private:
		/**
		 * Static as it's only image related utilities for now
		 */
		image() = default;
	};
}
