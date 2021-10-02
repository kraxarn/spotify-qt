#pragma once

#include "lib/spotify/entity.hpp"
#include "lib/enum/mediatype.hpp"
#include "lib/json.hpp"
#include "lib/enums.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * A show with episodes
		 */
		class show: public entity
		{
		public:
			/**
			 * Countries, in ISO 3166, which show can be played in
			 */
			std::vector<std::string> available_markets;

			/**
			 * Description in plain text
			 */
			std::string description;

			/**
			 * If show has explicit content
			 * @note false if unknown
			 */
			bool is_explicit = false;

			/**
			 * External URLs
			 */
			std::map<std::string, std::string> external_urls;

			/**
			 * Link to the Web API endpoint
			 */
			std::string href;

			/**
			 * Description in HTML
			 */
			std::string html_description;

			/**
			 * Cover art
			 */
			std::string image;

			/**
			 * Hosted outside of Spotify's CDN
			 */
			bool is_externally_hosted = false;

			/**
			 * Languages used
			 */
			std::vector<std::string> languages;

			/**
			 * Media type
			 */
			lib::media_type media_type;

			/**
			 * Publisher
			 */
			std::string publisher;

			/**
			 * Spotify URI
			 */
			std::string uri;
		};

		/** JSON -> Show */
		void from_json(const nlohmann::json &j, show &s);
	}
}
