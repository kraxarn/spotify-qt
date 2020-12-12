#pragma once

#include "album.hpp"
#include "spotify.hpp"
#include "track.hpp"

namespace lib
{
	namespace spt
	{
		/**
		 * An artist
		 */
		class artist
		{
		public:
			/**
			 * Construct an empty artist
			 */
			artist() = default;

			/**
			 * Construct an artist from a json object
			 * @param json JSON
			 */
			explicit artist(const nlohmann::json &json);

			/**
			 * @deprecated Use spotify::artist::top_tracks instead
			 */
			std::vector<Track> topTracks(spotify &spotify) const;

			/**
			 * @deprecate Use spotify::artist::albums instead
			 */
			std::vector<Album> albums(spotify &spotify) const;

			/**
			 * @deprecated Use spotify::artist::related_artists instead
			 */
			std::vector<artist> relatedArtists(spotify &spotify) const;

			/**
			 * Format the current artist as a json object
			 * @return JSON
			 */
			nlohmann::json to_json() const;

			/**
			 * Number of followers
			 */
			int followers = 0;

			/**
			 * Popularity from 0-100
			 */
			int popularity = 0;

			/**
			 * Genres associated with the artist
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
			 * Cover art for artist
			 */
			std::string image;

			/**
			 * Known external URLs
			 */
			std::vector<std::pair<std::string, std::string>> external_urls;
		};
	}
}