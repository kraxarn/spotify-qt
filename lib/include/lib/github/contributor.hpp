#pragma once

#include "thirdparty/json.hpp"

namespace lib
{
	namespace gh
	{
		class contributor
		{
		public:
			/**
			 * Username
			 */
			std::string login;

			/**
			 * URL to avatar
			 */
			std::string avatar_url;
		};

		void from_json(const nlohmann::json &j, contributor &c);
	}
}
