#pragma once

#include "thirdparty/json.hpp"

#include <string>

namespace lib
{
	namespace spt
	{
		/**
		 * Spotify context
		 */
		class context
		{
		public:
			context() = default;

			/**
			 * Spotify URI of context
			 */
			std::string uri;

			/**
			 * Type of context
			 */
			std::string type;
		};

		/**
		 * json -> context
		 */
		void from_json(const nlohmann::json &j, context &p);
	}
}