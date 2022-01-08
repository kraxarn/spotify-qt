#pragma once

#include "lib/search/link.hpp"

namespace lib
{
	namespace ddg
	{
		class results
		{
		public:
			results() = default;

			/**
			 * Basic artist information and social links
			 */
			std::vector<lib::ddg::link> content;

			/**
			 * Metadata about search
			 */
			std::vector<lib::ddg::link> meta;

			/**
			 * Source for abstract text
			 */
			std::string abstract_source;

			/**
			 * Source for abstract text
			 */
			std::string abstract_url;
		};

		/** JSON -> Results */
		void from_json(const nlohmann::json &j, results &r);
	}
}
