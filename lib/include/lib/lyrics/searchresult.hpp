#pragma once

#include "thirdparty/json.hpp"

namespace lib
{
	namespace lrc
	{
		/**
		 * Lyrics search result
		 */
		class search_result
		{
		public:
			/**
			 * Lyrics ID for fetching actual lyrics
			 */
			int lyrics_id;
		};

		void from_json(const nlohmann::json &json, search_result &result);
	}
}
