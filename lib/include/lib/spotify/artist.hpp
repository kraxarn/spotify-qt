#pragma once

#include "thirdparty/json.hpp"

#include <string>
#include <vector>
#include <map>

namespace lib
{
	namespace spt
	{
		class artist
		{
		public:
			artist() = default;

			/**
			 * Amount of followers
			 */
			int followers = 0;

			/**
			 * Popularity from 0-100, 100 being the most popular
			 */
			int popularity = 0;

			/**
			 * List of artist genres
			 */
			std::vector<std::string> genres;

			/**
			 * Spotify ID
			 */
			std::string id;

			/**
			 * Name of artist
			 */
			std::string name;

			/**
			 * URL to cover image
			 */
			std::string image;

			/**
			 * Known external URLs
			 */
			std::map<std::string, std::string> external_urls;
		};

		/**
		 * Artist -> JSON
		 */
		void to_json(nlohmann::json &j, const artist &a);

		/**
		 * JSON -> Artist
		 */
		void from_json(const nlohmann::json &j, artist &a);
	}
}
