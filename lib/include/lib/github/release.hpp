#pragma once

#include "thirdparty/json.hpp"

namespace lib
{
	namespace gh
	{
		class release
		{
		public:
			/**
			 * Release body
			 */
			std::string body;
		};

		void from_json(const nlohmann::json &j, release &r);
	}
}
