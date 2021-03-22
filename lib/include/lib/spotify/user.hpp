#pragma once

#include "thirdparty/json.hpp"

#include <string>

namespace lib
{
	namespace spt
	{
		class user
		{
		public:
			user() = default;

			/**
			 * Display name
			 */
			std::string display_name;

			/**
			 * User ID
			 */
			std::string id;

			/**
			 * URL to user avatar
			 */
			std::string image;

			/**
			 * Spotify subscription type, like premium, free, etc.
			 */
			std::string product;
		};

		/**
		 * user -> json
		 */
		void to_json(nlohmann::json &j, const user &u);

		/**
		 * json -> user
		 */
		void from_json(const nlohmann::json &j, user &u);
	}
}
