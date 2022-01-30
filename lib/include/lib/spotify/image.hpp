#pragma once
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
			 * Size of small images, 64x64
			 */
			static constexpr int size_small = 64;

			/**
			 * Size of large images, 300x300
			 */
			static constexpr int size_large = 300;
		};

		void from_json(const nlohmann::json &j, image &i);

		void to_json(nlohmann::json &j, const image &i);
	}
}
