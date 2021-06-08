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
		 * Data is a JPEG image
		 * @param data
		 * @return
		 */
		static auto is_jpeg(const std::vector<unsigned  char> &data) -> bool;

	private:
		/**
		 * Static as it's only image related utilities for now
		 */
		image() = default;
	};
}
