#pragma once

#include "lib/spotify/entity.hpp"
#include "lib/json.hpp"
#include "lib/spotify/track.hpp"
#include "lib/spotify/show.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * A specific episode of a show
		 */
		class episode: public entity
		{
		public:
			/**
			 * 30 second MP3 preview
			 */
			std::string audio_preview_url;

			/**
			 * Description in plain text
			 */
			std::string description;

			/**
			 * Length in milliseconds
			 */
			int duration_ms = 0;

			/**
			 * If episode has explicit content
			 * @note false if unknown
			 */
			bool is_explicit = false;

			/**
			 * External URLs
			 */
			std::map<std::string, std::string> external_urls;

			/**
			 * Link ot the Web API endpoint
			 */
			std::string href;

			/**
			 * Description in HTML
			 */
			std::string html_description;

			/**
			 * URL to cover art
			 */
			std::string image;

			/**
			 * Hosted outside of Spotify's CDN
			 */
			bool is_externally_hosted = false;

			/**
			 * Playable in current market
			 */
			bool is_playable = false;

			/**
			 * Languages used in ISO 639 codes
			 */
			std::vector<std::string> languages;

			/**
			 * Date when first released
			 * @note Varying precision
			 */
			std::string release_date;

			/**
			 * Precision of release_date
			 */
			std::string release_date_precision;

			/**
			 * Spotify URI
			 */
			std::string uri;

			/**
			 * Convert to track entity, mostly for compatibility
			 */
			auto to_track(const lib::spt::show &show) const -> lib::spt::track;
		};

		/** Episode -> JSON */
		void to_json(nlohmann::json &j, const episode &e);

		/** JSON -> Episode */
		void from_json(const nlohmann::json &j, episode &e);
	}
}
