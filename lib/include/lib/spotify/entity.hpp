#pragma once

#include "thirdparty/json.hpp"

#include <string>

namespace lib
{
	namespace spt
	{
		/**
		 * An entity with an ID and a name
		 */
		class entity
		{
		public:
			entity() = default;

			/**
			 * Entity ID
			 */
			std::string id;

			/**
			 * Entity name
			 */
			std::string name;
		};

		/**
		 * entity -> json
		 */
		void to_json(nlohmann::json &j, const entity &e);

		/**
		 * json -> entity
		 */
		void from_json(const nlohmann::json &j, entity &e);
	}
}
