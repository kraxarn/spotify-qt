#pragma once

#include "lib/enum/imagesize.hpp"

#include <thirdparty/json.hpp>

namespace lib
{
	namespace spt
	{
		class image
		{
		public:
			image() = default;

			/**
			 * Source URL
			 */
			std::string url;

			/**
			 * Image height in pixels
			 */
			int height = 0;

			/**
			 * Image width in pixels
			 */
			int width = 0;

			/**
			 * Get image size
			 */
			auto size() const -> lib::image_size;

			/**
			 * If image is of specific size
			 * @param size Size to check
			 * @return Image is of size
			 */
			auto is_size(lib::image_size image_size) const -> bool;
		};

		void from_json(const nlohmann::json &j, image &i);

		void to_json(nlohmann::json &j, const image &i);
	}
}
