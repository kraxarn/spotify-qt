#pragma once

#include "lib/strings.hpp"

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
			virtual ~entity() = default;

			/**
			 * Manually construct an entity
			 */
			entity(std::string id, std::string name);

			/**
			 * Entity ID
			 */
			std::string id;

			/**
			 * Entity name
			 */
			std::string name;

			/**
			 * Check if entity is valid,
			 * cam be overridden with custom checks
			 */
			virtual auto is_valid() const -> bool;

			/**
			 * Combine names to a comma separated string
			 * @param entities Entities to combine
			 */
			static auto combine_names(const std::vector<entity> &entities) -> std::string;

			/**
			 * Combine names with separator
			 * @param entities Entities to combine
			 */
			static auto combine_names(const std::vector<entity> &entities,
				const char *separator) -> std::string;
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
