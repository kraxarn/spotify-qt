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
		};

		/** JSON -> Results */
		void from_json(const nlohmann::json &j, results &r);
	}
}
