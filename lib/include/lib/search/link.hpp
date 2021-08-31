#pragma once

#include "lib/enum/artistprofile.hpp"
#include "lib/format.hpp"

#include <string>

namespace lib
{
	namespace ddg
	{
		class link
		{
		public:
			link() = default;

			/**
			 * Human readable label for link
			 */
			std::string label;

			/**
			 * Type of link
			 */
			lib::artist_profile data_type = lib::artist_profile::unknown;

			/**
			 * Value, usually ID for service
			 */
			std::string value;

			/**
			 * Get link to profile page
			 */
			auto url() const -> std::string;
		};

		/** JSON -> Link */
		void from_json(const nlohmann::json &j, link &l);
	}
}
