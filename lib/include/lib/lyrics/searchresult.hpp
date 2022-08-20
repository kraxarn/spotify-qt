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

			/**
			 * Name of track
			 */
			std::string track;

			/**
			 * Name of artists
			 */
			std::vector<std::string> artists;

			/**
			 * Name of album
			 */
			std::string album;
		};

		void from_json(const nlohmann::json &json, search_result &result);
	}
}
