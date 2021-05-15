#pragma once

#include "thirdparty/json.hpp"

#include <string>

namespace lib
{
	namespace spt
	{
		/**
		 * Track info, like lyrics
		 */
		class track_info
		{
		public:
			track_info() = default;

			std::string lyrics;

			auto is_valid() const -> bool;
		};

		/**
		 * lyrics -> json
		 */
		void to_json(nlohmann::json &j, const track_info &l);

		/**
		 * json -> lyrics
		 */
		void from_json(const nlohmann::json &j, track_info &l);
	}
}
